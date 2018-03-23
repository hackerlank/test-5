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
	// ��ֹ�ڲ���������ɵľ��й©
	// ��������send��close�������õ������
	// ��Ϊsend�����ݻ��close������������0 ������Ϣ����һ��
	// ���¶Զ�ֻ���յ�send���ȵ����ݣ������ᴥ��0���ȵ�close
	void 	check_time_out(int time_now); 
	static void * time_thread(void * object);	//ʱ���߳�
public:
	GameLogicBase* m_logic_processor;
	mm_timer m_mm_timer;
	
private:
	int					m_gw_count;
	int 				m_gw_fd[MAX_GW_COUNT];		//�������صľ��

	int 				m_epoll_fd; 				//����epoll���
	int 				m_listen_fd;				//�򿪵Ķ˿ھ��
	
	map<int, Connect_Session_t*> m_connect_session; //ÿ�����ӵ����ݻ�����
};
	
#endif
	

