#ifndef __GAME_GAMEBASE_H__
/*
desc:��Ϸ������
writer:Lake
*/
#define __GAME_GAMEBASE_H__

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <Log.h>
#include "Options.h"
#include <Net.h>
#include <Thread.h>
#include "Pubdata.h"
#include <mysql.h>
#include "redisclient.h"

#define		TIME_WAITING		61
#define		DESK_COUNT			150

using namespace std;
//�û���Ϣ
class UserGameInfoBase
{
	public:
		unsigned int	uid;		//�û�id
		int				desk_id;	//����id
		unsigned int	coin;		//������Ϸ��
		unsigned int	integral;	//����
		unsigned int	ingotnum;	//Ԫ��
		string 			nickname;	//�ǳ�
		unsigned char	offline;	// 1:���ߣ�2:����
		string 			headurl;	//ͷ���ַ
		unsigned char 	gender;		//�Ա� 
		int 			gamevalue;	//��������
		unsigned int 	matchid;	//����id
		bool			ifout;		//�Ƿ���� 1 ����
		unsigned int 	gameid;		//��Ϸid
		unsigned int 	roomcoin;	//����Я����������
		unsigned char	getcoinnum;	//��ó������
		int				robotwincoin;//�Ի�������Ӯֵ
		unsigned int	usercurwinrate;//�û���ǰӮǮ���ʣ���ֱ�
		unsigned int	gametruns;//��Ϸ����
	public:
		UserGameInfoBase(){
			uid = 0;
			desk_id = -1;
			coin = 0;
			integral = 0;
			ingotnum = 0;
			gamevalue = 0;
			matchid = 0;
			ifout = false;
			gameid = 0;
			roomcoin = 0;
			getcoinnum = 0;
			offline = 1;
			robotwincoin = 0;
			usercurwinrate = 0;
			gametruns = 0;
		}
		~UserGameInfoBase(){
		}
		//Ǯ�ҼӼ�
		virtual int opcoin(unsigned int tcoin ,unsigned char op)
		{
			int lesscoin = 0;
			if(op==1){
				coin += tcoin;
			}else if(op==2){
				if(coin >= tcoin){
					coin -= tcoin;
				}else{
					lesscoin = tcoin - coin;
					coin = 0;
				}
			}
			return lesscoin;
		}
		//�����û�ʤ��
		void updateUserWinRate(int usermaxwincoin,int usermaxlosecoin)
		{
			int baseprob = abs(usermaxwincoin - usermaxlosecoin);
			int curbase = abs(usermaxlosecoin)-robotwincoin;
			if(curbase<=0){
				curbase = 0;
			}
			if(curbase>baseprob){
				curbase = baseprob;
			}
			if(baseprob<=0){
				usercurwinrate = 0;
			}else{
				usercurwinrate = (float)curbase/(float)baseprob * 10000;
			}
		}
		//����
		void reset()
		{
			desk_id = -1;
			gamevalue = 0;
			gameid = 0;
			matchid = 0;
			ifout = false;
		}
};

//��Ϸ�߼�����
class GameLogicBase
{
	public:
		GameLogicBase()
		{
			m_robotwinrate = 0;
			m_usermaxwincoin = 0;
			m_usermaxlosecoin = 0;
			m_robotmaxlosecoin = 0;
			m_robotmaxwincoin = 0;
			m_robotcurwincoin = 0;
			m_robotcurwinrate = 0;
		}
		~GameLogicBase(){}
		static void * queue_consume(void * object);	//�����߳�
		static void * time_pthread(void * object);//ʱ���߳�
	protected:
		int req_settle(int user_id, char add_reduce, int coin,char logtype,char coin_type);//����������ֽ���
		int req_kick_user(int user_id);//��������
		int connect_lobby();//���Ӵ���������
	protected:
		//��ʼ��
		virtual int init()=0;
		//��Ϣ������
		virtual int process_one_msg(queue_msg *data)=0;
		//ʱ���̵߳���ִ�к���
		virtual void process_time_thread(queue_msg* data)=0;
		//���Ӷ�ʱhandler����
		virtual void process_time(int desk_id,queue_msg *data) = 0;
		//�û����뷿��
		virtual int process_user_join(queue_msg *data){return 0;}
		//�û��뿪�����������뿪
		virtual void process_user_left(queue_msg *data) = 0;
		//�����Ϸ�ң������������
		virtual void process_get_coin(queue_msg *data) = 0;
		//�û����� �û�˽��
		virtual void process_user_chat(queue_msg *data) = 0;
		//�û����� �û�����
		virtual void process_user_free_chat(queue_msg *data) = 0;
		//��������ʹ��
		virtual void process_user_showchat(queue_msg *data) = 0;
		//�û���ʼ׼��
		virtual void process_user_begin(queue_msg *data) = 0;
		//����
		virtual void process_user_continue(queue_msg *data) = 0;
		//����
		virtual void process_user_change_desk(queue_msg *data) = 0;
		//��¼�û�����Ӯ��������ڼ���ʤ��
		virtual int update_win_lose(queue_msg *data,int user_id, int win_lose) = 0;
		//���ش������
		virtual void resp_game_error(int uid, int retcode) = 0;
	protected:
		//�������ݿ��г�ʼ������
		void loadDbInitData();
		//��ȡ�û���Ϣ
		void get_user_info(string userkey,UserGameInfoBase& user_status_info);
		//���»�����ʤ��
		void updateRobotWinRate(queue_msg *data);
		//���»�������Ӯ���
		void updateRobotLoseWincoin();
	public:
		CThreadQueue 				m_thread_queue;		//��Ϣ�б�
		void* 						p_m_mm_framework;	//��Ϸ����ͨѶ�ܹ�
	protected:
		int							m_scoket_lobby;		//�������
		map<int, UserGameInfoBase> 	m_map_user;			//�û���Ϣ
		MYSQL 						m_mysql;			//���ݿ���
		RedisClient* 				redisClient;		//redis �ͻ���
		
		unsigned int		m_min_coin;	//�׷�
		unsigned int		m_coin_limit;//�ⶥ
		unsigned int 		m_enter_limit;//��������
		unsigned int		m_commission;//��ˮ����
		unsigned int 		m_basefee;//̨��
		unsigned char		m_coin_type;//�������� 1 ��� 2 ����
		unsigned int		m_basetimes;//��������
		unsigned short		m_gametype;//��Ϸ����
		unsigned int		m_baseintegral;//���ֵ׷�
		unsigned char		m_roomlevel;//����ȼ�
		unsigned int		m_robotwinrate;//������Ӯ�ĸ��� ��ֱ�
		int					m_usermaxlosecoin;//�û�������Ǯ�����ڼ���ʤ��
		int					m_usermaxwincoin;//�û����Ӯ��Ǯ�����ڼ���ʤ��
		int					m_robotmaxlosecoin;//���л�����������Ǯ�����ڼ���ʤ��
		int					m_robotmaxwincoin;//���л��������Ӯ��Ǯ�����ڼ���ʤ��
		int					m_robotcurwincoin;//���л����˵�ǰӮ��Ǯ�����ڼ���ʤ��
		unsigned int		m_robotcurwinrate;//�����˵�ǰӮǮ���ʣ���ֱ� ��ǰΪ�û�Ӯ�ĸ���
};


#endif

