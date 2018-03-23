#include <sys/socket.h>
#include <errno.h>
#include <sys/epoll.h>
#include <error.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "mm_framework.h"
	
using namespace std;

void mm_timer::Run(void *p_mm_framework)
{
	mm_framework *p_mm_framework_tmp = (mm_framework*)p_mm_framework;

	memset(data, 0, sizeof(data));
	queue_msg data_msg;
	data_msg.fd = 0;
	data_msg.IsInner = PROCESS_TIME;
	data_msg.length = LEN_PKG_HEAD + 4;			
	pkg_head pkg_head_input;
	pkg_head_input.command = 1;
	pkg_head_input.packet(data_msg.recv_mb);
	
	time_now = 0;
	struct timeval tempval;
	while (!stopped)
	{
		tempval.tv_sec = 1;
		tempval.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tempval);
		pthread_mutex_lock(&m_lock_timmer);
		for(int i=0; i<DESK_COUNT; i++)
		{
			if(data[time_now][i] == 0)
			{
				break;
			}
			else
			{
				int desk_id = data[time_now][i] -1;
				memcpy(data_msg.recv_mb + LEN_PKG_HEAD, &desk_id, 4);		
				p_mm_framework_tmp->m_logic_processor->m_thread_queue.putq(&data_msg);
				data[time_now][i] = 0;
			}
		}
		pthread_mutex_unlock(&m_lock_timmer);
		
		time_now++;
		if(time_now >= TIME_WAITING)
		{
			time_now = 0;
		}
	}
}

int mm_timer::update_timer(int desk_id, int delay_time)
{
	pthread_mutex_lock(&m_lock_timmer);
	for(int i=0; i<TIME_WAITING; i++)
	{
		for(int j=0; j<DESK_COUNT; j++)
		{
			if(data[i][j] == 0)
			{
				break;
			}
			if(data[i][j] == (desk_id+1))
			{
				for(int k=j; k<DESK_COUNT-1; k++)
				{
					if(data[i][k+1] != 0)
					{
						data[i][k] = data[i][k+1];
						continue;
					}
					else
					{
						data[i][k] = 0;
						break;
					}
				}
				i = TIME_WAITING;
				break;
			}
		}
	}

	int next_time = time_now + delay_time;
	while(next_time >= TIME_WAITING)
	{
		next_time -= TIME_WAITING;
	}
	for(int i=0; i<DESK_COUNT; i++)
	{
		if(data[next_time][i] == 0)
		{
			data[next_time][i] = desk_id+1;
			break;
		}
	}
	pthread_mutex_unlock(&m_lock_timmer);
	return 0;
}



int mm_framework::Init()
{
	m_epoll_fd = 0;
	m_listen_fd = 0;
	for(int i=0; i<MAX_GW_COUNT; i++)
		m_gw_fd[i] = 0;

	m_epoll_fd = epoll_create(MAXFDS);
	if(m_epoll_fd<0)
	{
		LOG (LOG_ERROR, __FILE__, __LINE__, "epoll_create error");
		return -1;
	}

	m_gw_count = ini.gw_count;
	check_gw_connect();

	if(ini.listen_count == 1)
	{
		m_listen_fd= open_tcp_port(ini.listen_ip, ini.listen_port, ini.backlog);
		if(m_listen_fd < 0)
		{
			LOG (LOG_ERROR, __FILE__, __LINE__, "open port[%s:%d] error", ini.listen_ip, ini.listen_port);
			return -1;
		}
		// 侦听端口千万不要设置为ET模式
		struct epoll_event ev;
		ev.events = EPOLLIN|EPOLLERR|EPOLLHUP|EPOLLPRI;
		ev.data.fd = m_listen_fd;
		if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_fd, &ev) != 0)
		{
			LOG (LOG_ERROR, __FILE__, __LINE__, "epoll_ctl error");
			return -1;
		}
	}
	
	pthread_t pid;
	m_logic_processor->p_m_mm_framework = this;
	pthread_create(&pid,0,m_logic_processor->queue_consume,m_logic_processor);
	pthread_create(&pid,0,time_thread,this);
	pthread_create(&pid,0,m_logic_processor->time_pthread,m_logic_processor);

	return 0;
}

void * mm_framework::time_thread(void * object)
{
	pthread_detach(pthread_self());
	mm_framework *pWork = (mm_framework *)object;
	pWork->m_mm_timer.Run(pWork);
	return NULL;
}


int mm_framework::Run()
{
	int check_times = 0;
	unsigned int last_time = time(NULL);
	
	struct epoll_event events[MAXFDS];
	while (!stopped)
	{
		int nfds = epoll_wait(m_epoll_fd, events, MAXFDS, 1000); 
		if( nfds <= 0)
		{
			continue;
		}
		for( int i = 0; i < nfds; i++ )
		{
			if(events[i].events & EPOLLIN)
			{
				if( events[i].data.fd == m_listen_fd)
				{
					handle_accept(events[i].data.fd);
				}
				else
				{
					//LOG (LOG_DEBUG, __FILE__, __LINE__, "handle_recv_tcp, fd=%d", events[i].data.fd);
					handle_recv_tcp(events[i].data.fd);
				}
			}
			else
			{
				handle_tcp_close(events[i].data.fd);
			}
		}

		check_times += 1;
		if(check_times>50)
		{
			unsigned int now_time = time(NULL);
			if((now_time - last_time) > 30)
			{
				last_time = now_time;
				check_times = 0;
				check_gw_connect();
				check_time_out(now_time);
			}
		}
	}

	return 0;
}

int mm_framework::handle_accept(int listen_fd)
{
	int peer_fd;
	struct sockaddr_in peer;

	peer_fd = accept_tcp (listen_fd, &peer);
	if (peer_fd < 0)
		return -1;

	long long id = CONNECTION_ID (peer.sin_addr.s_addr, peer.sin_port, peer_fd);
	Connect_Session_t* ss = new Connect_Session_t (id, TCP_STREAM);
	if (ss->recv_mb == NULL || ss->send_mb == NULL)
	{
		LOG (LOG_ERROR, __FILE__, __LINE__, "malloc session buffer failed");
		delete ss;
		return -1;
	}
	ss->is_inner = INNER_CONNECT;

	struct epoll_event ev;
	ev.events = EPOLLIN|EPOLLERR|EPOLLHUP|EPOLLPRI;
	ev.data.fd = peer_fd;
	if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, peer_fd, &ev) < 0)
	{
		LOG (LOG_ERROR, __FILE__, __LINE__, "epoll_ctl error");
		delete ss;
		return -1;
	}
	else
	{
		m_connect_session.insert (make_pair (peer_fd, ss));
	}

	return 0;
}


int mm_framework::handle_recv_tcp(int perr_fd)
{
	Connect_Session_t* s= NULL;
	map<int,Connect_Session_t*>::iterator pos;
	pos = m_connect_session.find (perr_fd);
	if (pos == m_connect_session.end ())
		return -1;
	else
		s = pos->second;

	if (recv_tcp_buffer (s) < 0)
	{
		handle_tcp_close(perr_fd);
		return -1;
	}

	pkg_head pkg_head_input;
	pkg_head_input.unpacket(s->recv_mb);
	if(pkg_head_input.length < LEN_PKG_HEAD || pkg_head_input.length > ini.socket_bufsize)
	{
		handle_tcp_close(perr_fd);
		return -1;
	}

	while(1)
	{
		if (s->recv_len < LEN_PKG_HEAD)
		{
			return 0;
		}

		pkg_head pkg_head_input;
		pkg_head_input.unpacket(s->recv_mb);
		if(pkg_head_input.length < LEN_PKG_HEAD || pkg_head_input.length > ini.socket_bufsize)
		{
			handle_tcp_close(perr_fd);
			return -1;
		}
		if(pkg_head_input.length > s->recv_len)
			return 0;

		
		//LOG (LOG_DEBUG, __FILE__, __LINE__, "[fd=%d][%d:%d:%d:%d]",
			//perr_fd, pkg_head_input.length, pkg_head_input.type, pkg_head_input.command, pkg_head_input.uid);

		//把消息写到队别中
		queue_msg data;
		data.fd = perr_fd;
		data.IsInner = s->is_inner;
		data.length = pkg_head_input.length;
		memcpy(data.recv_mb, s->recv_mb, pkg_head_input.length);
		m_logic_processor->m_thread_queue.putq(&data);

		if(pkg_head_input.length == s->recv_len)
		{
			s->recv_len = 0;
			return 0;
		}
		else
		{
			s->recv_len -= pkg_head_input.length;
			memmove(s->recv_mb, s->recv_mb + pkg_head_input.length, s->recv_len);
		}
	}
	
	return 0;
}

int mm_framework::handle_tcp_close(int fd)
{
	map<int,Connect_Session_t*>::iterator pos = m_connect_session.find (fd);
	if (pos == m_connect_session.end())
	{
		LOG (LOG_ERROR, __FILE__, __LINE__, "remove session,can't find session map");
	}
	else
	{
		if(pos->second->is_inner == OUT_CONNECT)
		{
			for(int i=0; i<m_gw_count; i++)
			{
				if(m_gw_fd[i] == fd)
				{
					LOG (LOG_ERROR, __FILE__, __LINE__, "gw[%s:%d] lost connection!", ini.gw[i].ip_gw, ini.gw[i].port_gw);
					m_gw_fd[i] = 0;
					break;
				}
			}
		}
		delete pos->second;
		m_connect_session.erase (pos);
	}
	close(fd);
	
	return 0;
}
void mm_framework::check_gw_connect()
{
	for(int i=0; i<m_gw_count; i++)
	{
		if(m_gw_fd[i] <= 0)
		{
			int gw_fd = connect_gw(ini.gw[i].ip_gw, ini.gw[i].port_gw);
			if(gw_fd < 0)
			{
				LOG (LOG_ERROR, __FILE__, __LINE__, "connect gw[%s:%d] error", ini.gw[i].ip_gw, ini.gw[i].port_gw);
				continue;
			}

			struct epoll_event ev;
			ev.events = EPOLLIN|EPOLLET|EPOLLERR|EPOLLHUP|EPOLLPRI;
			ev.data.fd = gw_fd;
			if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, gw_fd, &ev) == 0)
			{
				m_gw_fd[i] = gw_fd;
			}
			else
			{
				LOG (LOG_ERROR, __FILE__, __LINE__, "epoll_ctl error");
				continue;
			}
		}		
	}
}

int mm_framework::connect_gw(char* ip, int port)
{
	int sockfd = 0;
	struct	sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;	
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connect (sockfd, (const struct sockaddr *)&servaddr, sizeof (servaddr)) != 0)	
	{
		LOG (LOG_ERROR,  __FILE__, __LINE__, "connect gw[%s][%d] fail! error[%s]", ip, port, strerror(errno));
		return -1;
	}

	struct timeval timeout = {1, 0};
	setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)& timeout, sizeof (struct timeval));

	//set socket buffer
	int bufsize = ini.socket_bufsize_gw;
	setsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&bufsize, sizeof (int));
	setsockopt (sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize, sizeof (int));

	pkg_head pkg_connect_gw_header;
	pkg_connect_gw_header.length = LEN_PKG_HEAD + 2;
	pkg_connect_gw_header.type = TYPE_GW;
	pkg_connect_gw_header.command = COMMAND_ROOM_REGISTER;
	
	char sBuffer[LEN_PKG_HEAD+2+1]= {0};
	int nLen = LEN_PKG_HEAD+2;
	pkg_connect_gw_header.packet(sBuffer);
	unsigned short self_type = ini.server_type;
	memcpy(sBuffer+LEN_PKG_HEAD, &self_type, 2);
	if ( nLen != send (sockfd, sBuffer, nLen, 0))		
	{
		LOG (LOG_DEBUG,  __FILE__, __LINE__, "send gw fail!");
		close (sockfd); 
		return -1;
	}

	nLen = LEN_PKG_HEAD + 4;
	memset(sBuffer, 0 , sizeof(sBuffer));
	if(nLen != recv (sockfd, sBuffer, nLen, 0))
	{
		LOG (LOG_ERROR, __FILE__, __LINE__,  "recv gw[%s][%d] error!", ip, port);
		close (sockfd);
		return -1;
	}
	else
	{
		int nReturn = 0;
		memcpy(&nReturn, sBuffer+LEN_PKG_HEAD, 4);
		if(nReturn != 0)
		{
			LOG (LOG_ERROR, __FILE__, __LINE__,  "connect gw return != 0, maybe type error");
			close (sockfd);
			return -1;
		}
	}

	int val = fcntl(sockfd, F_GETFL, 0); 
	val |= O_NONBLOCK; 
	fcntl (sockfd, F_SETFL, val);

	Connect_Session_t* ss = new Connect_Session_t (sockfd, TCP_STREAM);
	if(ss == NULL)
		return -1;
	ss->is_inner = OUT_CONNECT;
	m_connect_session.insert (make_pair (sockfd, ss));

	LOG (LOG_DEBUG,  __FILE__, __LINE__, "connect gw[fd=%d:%s:%d] OK!",sockfd, ip, port);
	return sockfd;
}

int	mm_framework::send_gw(int uid, char* buffer, int length)
{
	if(uid < MAX_AI_NPC_UID)
		return 0;
	int gw_index = uid%m_gw_count;
	if(m_gw_fd[gw_index] <= 0)
		return -1;
	if(length != socket_send(m_gw_fd[gw_index], buffer, length))		
	{
		LOG (LOG_ERROR,  __FILE__, __LINE__, "send_gw[%d] error!", gw_index);
		close(m_gw_fd[gw_index]);
		m_gw_fd[gw_index] = 0;
		return -1;
	}

	return 0;
}

void mm_framework::check_time_out(int time_now)
{
	map<int,Connect_Session_t*>::iterator pos;
	for (pos=m_connect_session.begin(); pos!=m_connect_session.end(); )	
	{
		Connect_Session_t* s = pos->second;
		if(time_now - s->stamp > 120 && s->is_inner == INNER_CONNECT)
		{
			close(CONNECTION_FD(s->id));
			delete s;
			m_connect_session.erase (pos++);
		}
		else
			pos++;
	}
}


