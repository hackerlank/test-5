#ifndef __GAME_GAMEBASE_H__
/*
desc:游戏基础类
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
//用户信息
class UserGameInfoBase
{
	public:
		unsigned int	uid;		//用户id
		int				desk_id;	//桌子id
		unsigned int	coin;		//房间游戏币
		unsigned int	integral;	//积分
		unsigned int	ingotnum;	//元宝
		string 			nickname;	//昵称
		unsigned char	offline;	// 1:在线，2:离线
		string 			headurl;	//头像地址
		unsigned char 	gender;		//性别 
		int 			gamevalue;	//比赛积分
		unsigned int 	matchid;	//比赛id
		bool			ifout;		//是否出局 1 出局
		unsigned int 	gameid;		//游戏id
		unsigned int 	roomcoin;	//房间携带货币数量
		unsigned char	getcoinnum;	//获得筹码次数
		int				robotwincoin;//对机器人输赢值
		unsigned int	usercurwinrate;//用户当前赢钱概率，万分比
		unsigned int	gametruns;//游戏局数
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
		//钱币加减
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
		//更新用户胜率
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
		//重置
		void reset()
		{
			desk_id = -1;
			gamevalue = 0;
			gameid = 0;
			matchid = 0;
			ifout = false;
		}
};

//游戏逻辑基类
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
		static void * queue_consume(void * object);	//消费线程
		static void * time_pthread(void * object);//时间线程
	protected:
		int req_settle(int user_id, char add_reduce, int coin,char logtype,char coin_type);//请求大厅币种结算
		int req_kick_user(int user_id);//请求踢人
		int connect_lobby();//连接大厅服务器
	protected:
		//初始化
		virtual int init()=0;
		//消息处理函数
		virtual int process_one_msg(queue_msg *data)=0;
		//时间线程单独执行函数
		virtual void process_time_thread(queue_msg* data)=0;
		//桌子定时handler函数
		virtual void process_time(int desk_id,queue_msg *data) = 0;
		//用户加入房间
		virtual int process_user_join(queue_msg *data){return 0;}
		//用户离开，大厅请求离开
		virtual void process_user_left(queue_msg *data) = 0;
		//获得游戏币，大厅请求更新
		virtual void process_get_coin(queue_msg *data) = 0;
		//用户聊天 用户私聊
		virtual void process_user_chat(queue_msg *data) = 0;
		//用户聊天 用户公聊
		virtual void process_user_free_chat(queue_msg *data) = 0;
		//交互道具使用
		virtual void process_user_showchat(queue_msg *data) = 0;
		//用户开始准备
		virtual void process_user_begin(queue_msg *data) = 0;
		//继续
		virtual void process_user_continue(queue_msg *data) = 0;
		//换桌
		virtual void process_user_change_desk(queue_msg *data) = 0;
		//记录用户的输赢结果，用于计算胜率
		virtual int update_win_lose(queue_msg *data,int user_id, int win_lose) = 0;
		//返回错误代码
		virtual void resp_game_error(int uid, int retcode) = 0;
	protected:
		//加载数据库中初始化数据
		void loadDbInitData();
		//获取用户信息
		void get_user_info(string userkey,UserGameInfoBase& user_status_info);
		//更新机器人胜率
		void updateRobotWinRate(queue_msg *data);
		//更新机器人输赢金币
		void updateRobotLoseWincoin();
	public:
		CThreadQueue 				m_thread_queue;		//消息列表
		void* 						p_m_mm_framework;	//游戏网关通讯架构
	protected:
		int							m_scoket_lobby;		//大厅句柄
		map<int, UserGameInfoBase> 	m_map_user;			//用户信息
		MYSQL 						m_mysql;			//数据库句柄
		RedisClient* 				redisClient;		//redis 客户端
		
		unsigned int		m_min_coin;	//底分
		unsigned int		m_coin_limit;//封顶
		unsigned int 		m_enter_limit;//进入限制
		unsigned int		m_commission;//抽水比例
		unsigned int 		m_basefee;//台费
		unsigned char		m_coin_type;//货币类型 1 金币 2 积分
		unsigned int		m_basetimes;//基础倍数
		unsigned short		m_gametype;//游戏类型
		unsigned int		m_baseintegral;//积分底分
		unsigned char		m_roomlevel;//房间等级
		unsigned int		m_robotwinrate;//机器人赢的概率 万分比
		int					m_usermaxlosecoin;//用户最大输的钱，用于计算胜率
		int					m_usermaxwincoin;//用户最大赢的钱，用于计算胜率
		int					m_robotmaxlosecoin;//所有机器人最大输的钱，用于计算胜率
		int					m_robotmaxwincoin;//所有机器人最大赢的钱，用于计算胜率
		int					m_robotcurwincoin;//所有机器人当前赢的钱，用于计算胜率
		unsigned int		m_robotcurwinrate;//机器人当前赢钱概率，万分比 当前为用户赢的概率
};


#endif

