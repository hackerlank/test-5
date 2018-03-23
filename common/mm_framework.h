#ifndef __MM_FRAMEWORK__
#define __MM_FRAMEWORK__
	
#include "Options.h"
#include "Pubdata.h"
#include "GameBase.h"
#include "Thread.h"
#include "redisclient.h"
#include "common_utils.h"
#include "pbc/userinfo.pb.h"
#include "Log.h"
#include "Common.h"
#include "Net.h"


class mm_timer
{
public:
	mm_timer()
	{
		pthread_mutex_init(&m_lock_timmer, NULL);
	}
	~mm_timer()
	{
		pthread_mutex_destroy(&m_lock_timmer);
	}
	void Run(void *p_mm_framework);
	int update_timer(int desk_id, int delay_time);
private:
	int time_now;
	int	data[TIME_WAITING][DESK_COUNT+1];
	pthread_mutex_t		m_lock_timmer;
};
	
class mm_framework
{
public:
	int Init();
	int Run();
	int		send_gw(int uid, char* buffer, int length);
	mm_framework()
	{
		
	}
	~mm_framework()
	{
		map<int,Connect_Session_t*>::iterator pos;
		for (pos=m_connect_session.begin(); pos!=m_connect_session.end(); ++pos)	
		{
			Connect_Session_t* s = pos->second;
			delete s;
			m_connect_session.erase (pos);
		}
		close(m_listen_fd);
		for(int i=0; i<m_gw_count; i++)
		{
			close(m_gw_fd[i]);
		}
		if(m_logic_processor != NULL){
			delete m_logic_processor;
		}
	};

private:
	int 	handle_accept(int listen_fd);
	int 	handle_recv_tcp(int gw_fd);
	int 	handle_tcp_close(int fd);
	int 	connect_gw(char* ip, int port);
	void	check_gw_connect();
	// 防止内部短连接造成的句柄泄漏
	// 尤其是在send和close连续调用的情况下
	// 因为send的数据会和close调用所触发的0 长度消息混在一起，
	// 导致对端只会收到send长度的数据，而不会触发0长度的close
	void 	check_time_out(int time_now); 
	static void * time_thread(void * object);	//时间线程
public:
	GameLogicBase* m_logic_processor;
	mm_timer m_mm_timer;
	
private:
	int					m_gw_count;
	int 				m_gw_fd[MAX_GW_COUNT];		//连接网关的句柄

	int 				m_epoll_fd; 				//外网epoll句柄
	int 				m_listen_fd;				//打开的端口句柄
	
	map<int, Connect_Session_t*> m_connect_session; //每个连接的数据缓冲区
};
	
#endif
	

