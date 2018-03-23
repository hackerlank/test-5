#ifndef __PUBDATA_H__
/*
desc:共用的数据结构
writer:Lake
*/
#define __PUBDATA_H__

#include <sstream>
#include <string.h>

#define	INNER_CONNECT			1	//内网
#define	OUT_CONNECT				2	//外网
#define PROCESS_TIME			3	//定时

#define LEN_PKG_HEAD 			10  //包头长度
#define TYPE_LOBBY				10	//大厅进程type
#define	MAX_GW_COUNT			10	//最大网关数量
#define TYPE_GW					1	//网关命令类型
#define	COMMAND_GW_REGISTER		1
#define COMMAND_GW_KICKUSER		3
#define COMMAND_ROOM_REGISTER   1



#define LEN_BUFF_DATA_50k	10240*5		//50k数据长度
#define LEN_BUFF_DATA_40k	10240*4		//40k数据长度
#define LEN_BUFF_DATA_30k	10240*3		//30k数据长度
#define LEN_BUFF_DATA_20k	10240*2		//20k数据长度
#define LEN_BUFF_DATA_10k	10240		//10k数据长度
#define LEN_BUFF_DATA_4k	4096		//4k数据长度
#define LEN_BUFF_DATA_2k	2048		//2k数据长度
#define LEN_BUFF_DATA_1k	1024		//1k数据长度
#define LEN_BUFF_DATA_512	512			//512byte数据长度
#define LEN_BUFF_DATA_256	256			//256byte数据长度
#define LEN_BUFF_DATA_128	128			//128byte数据长度
#define LEN_BUFF_DATA_64	64			//64byte数据长度
#define LEN_BUFF_DATA_32	32			//32byte数据长度
#define LEN_BUFF_DATA_16	16			//16byte数据长度
#define DATE_ONE_DAY_SECS	86400		//一天的秒数
#define DATE_ONE_HOUR_SECS	3600		//一小时的秒数
#define DATE_FIVE_MIN_SECS	300 		//5分钟秒数

#define MAX_AI_NPC_UID		10000		//npc用户最大id	

#define	LEN_SINGLE_MSG		8192		// 单个消息的最大长度
//公共命令代码
#define	COMMAND_USER_LOGIN				1		//用户登录
#define	COMMAND_USER_OFFLINE			2		//用户离线
#define	COMMAND_GAME_USER_CHAT			33		//房间聊天
#define	COMMAND_GAME_SHOWCHAT			900		//表情互动道具
#define COMMAND_GAME_ERROR				888 	//游戏错误
#define COMMAND_LOBBY_GET_COIN			2		//获取游戏币
#define	COMMAND_LOBBY_JOIN_ROOM			10		//加入房间
#define	COMMAND_LOBBY_LEFT_ROOM			11		//离开房间
#define	COMMAND_LOBBY_GAME_CHONGZHI		20		//充值
#define	COMMAND_LOBBY_GAME_GET_COIN		21		//游戏中更新游戏币
#define	COMMAND_LOBBY_GAME_USER_LEFT	22		//游戏中用户离开
#define	COMMAND_LOBBY_GAME_SETTLE		23		//游戏结算

#define	COMMAND_LOBBY_GAME_UPDATERWRATE	500		//更新房间机器人赢概率




//错误信息代码
#define ERROR_REDIS_DATA_NULL			100 //缓存数据为空
#define ERROR_COMMON_SERVER   			101  //服务器内部错误
#define ERROR_COMMON_REQUEST   			102  //请求参数错误
#define ERROR_COMMON_COIN_LESS   		103  //金豆不足
#define ERROR_COMMON_INGOT_LESS   		104  //元宝不足
#define ERROR_COMMON_INTEGRAL_LESS   	105  //积分不足
#define ERROR_LOBBY_ROOMNUM_MAX   		106  //房间人数已满
#define ERROR_LOBBY_ROOM_IN_GAME		107  //该用户原来所在的房间游戏没有结束
#define ERROR_COIN_LESS					108 //账号余额不足
#define ERROR_DESK_NO_EXIST				109 //桌子不存在
#define ERROR_DESK_NO_USER				110 //用户不存在
#define ERROR_REQUEST_PARAM				111 //请求参数出错
#define ERROR_COMMON_COIN_MORE   		112  //金豆超出房间限制
#define ERROR_COMMON_INTEGRAL_MORE   	113  //积分超出房间限制
#define ERROR_COMMON_ISGAMING   		115  //正在游戏中
#define ERROR_COMMON_POS_NO_EXIST   	116  //位置不存在
#define ERROR_COMMON_NO_POS   			117  //没有空余的位置可以坐下
#define ERROR_LOBBY_NO_ROOM				118 //大厅房间不存在
#define ERROR_MATCH_START				150 //比赛已经开始
#define ERROR_MATCH_END					151 //比赛已经结束
#define ERROR_MATCH_EXIST				152 //比赛不存在
#define ERROR_MATCH_IN					153 //正在比赛中
#define ERROR_MATCH_NO_APPLY			154 //没有报名比赛
#define ERROR_MATCH_NO_START			155 //比赛没有开始
#define ERROR_MATCH_APPLY_ALY			156 //已经报名当前比赛
#define ERROR_SELFROOM_CODE				170 //房间进入码错误
#define ERROR_SELFROOM_OUTOFDATE		171 //房间已经过期
#define ERROR_SELFROOM_NOSEFL			172 //只有房主才能操作


//读字节
#define ReadInt8(data,buf,buflen)      memcpy(data,buf,1); buflen += 1;
#define ReadInt16(data,buf,buflen)     memcpy(data,buf,2); buflen += 2;
#define ReadInt32(data,buf,buflen)     memcpy(data,buf,4); buflen += 4;
#define ReadInt64(data,buf,buflen)	    memcpy(data,buf,8); buflen += 8;
//写字节
#define WriteInt8(buf,data,lenvalue)    memcpy(buf,data,1); buf += 1; lenvalue += 1;
#define WriteInt16(buf,data,lenvalue)  memcpy(buf,data,2); buf += 2; lenvalue += 2;
#define WriteInt32(buf,data,lenvalue)  memcpy(buf,data,4); buf += 4; lenvalue += 4;
#define WriteInt64(buf,data,lenvalue)  memcpy(buf,data,8); buf += 8; lenvalue += 8;

using namespace std;

//货币类型
enum CoinType
{
	CoinType_Coin = 1,//金币
	CoinType_Integral//积分
};
//聊天频道类型
enum ChatType
{
	Chat_Sys = 1,
	Chat_World,
	Chat_Private
};
//经济变化log类型
enum LogType
{
	LogType_Win = 1,//赢
	LogType_Lose = 2,//输
	LogType_Basefee = 3,//台费
	LogType_Matchfee = 4,//比赛报名费
	LogType_MatchAward=5,//比赛排名奖励
	LogType_SHOPBUY=6,//商城购买金币
	LogType_MailAward=7,//邮件领取奖励
	LogType_ZJHBET =8,//炸金花押注
	LogType_UseMagicFace=9,//使用魔法表情
	LogType_TEXASBET =10,//德州押注
	LogType_TEXASIN =11,//德州携带金额进入
	LogType_TEXASOUT =12,//德州携带金额退出
	LogType_TEXASGIRL =13,//德州打赏荷官
	LogType_CHESSBET =14,//象棋下注
	LogType_CHESSOUT =15,//象棋返还
	LogType_MJ_Gang_Win = 16,//麻将杠赢
	LogType_MJ_Gang_Lose = 17,//麻将杠输
	LogType_MJ_HU_Win = 18,//麻将胡赢
	LogType_MJ_HU_Lose = 19,//麻将胡输
	LogType_MJ_HuaPig_Win = 20,//麻将花猪赢
	LogType_MJ_HuaPig_Lose = 21,//麻将花猪输
	LogType_MJ_NoTing_Win = 22,//麻将听牌赢
	LogType_MJ_NoTing_Lose = 23,//麻将听牌输
	LogType_MJ_WindRain_Win = 24,//麻将刮风下雨返还
	LogType_MJ_WindRain_Lose = 25,//麻将刮风下雨退还
	LogType_ROBOTADD =70,//机器人补充金额
	LogType_ROBOTRED =71,//机器人回收金额
	LogType_CHONGZHI=88	//充值
};
//游戏类型
enum GameType
{
	GameType_Ddz = 1,//斗地主
	GameType_Zjh = 2,//炸金花
	GameType_Nn = 3,//牛牛
	GameType_Texas = 4,//德州扑克
	GameType_SCMJ = 5,//四川麻将
	GameType_Chess = 6//中国象棋
};
//房间等级类型
enum RoomLvType
{
	RoomLvType_lV1 = 1,//初级
	RoomLvType_lV2 = 2,//中级
	RoomLvType_lV3 = 3,//高级
	RoomLvType_lV4 = 4//顶级
};

//比赛奖励信息
struct MatchAwardInfo
{
	unsigned int gametype;//游戏类型
	unsigned int roomlevel;//房间等级 1 初级 2 中级 3 高级...
	unsigned int lowrank;//排名下限
	unsigned int highrank;//排名上限
	unsigned int coin;//奖励金豆
	
	MatchAwardInfo(){
		gametype = 0;
		roomlevel = 0;
		highrank = 0;
		lowrank = 0;
		coin = 0;
	}
};
//商品信息
struct GoodsInfo
{
	unsigned int id;//商品id
	unsigned int price;//价格RMB
	unsigned int coin;//获得金豆
	unsigned int bonus;//额外获得金豆
	GoodsInfo()
	{
		id = 0;
		price = 0;
		coin = 0;
		bonus = 0;
	}
};
//牌概率信息
struct CardProbInfo
{
	unsigned int cardid;//牌id
	unsigned int normalprob;//一般概率
	unsigned int goodprob;//好的概率
	unsigned int badprob;//坏的概率
	CardProbInfo()
	{
		cardid = 0;
		normalprob = 0;
		goodprob = 0;
		badprob = 0;
	}
};

#pragma pack(1)
typedef struct _pkg_head
{
	unsigned short		length;//包长
	unsigned short		type;//类型
	unsigned short		command;//命令
	unsigned int		uid;//用户id

	unsigned int 		nowtime;//当前时间
	_pkg_head()
	{
		nowtime = time(NULL);
	}
	void packet(char *pOutput)
	{
		memcpy(pOutput, &length, 2);
		memcpy(pOutput+2, &type, 2);
		memcpy(pOutput+4, &command, 2);
		memcpy(pOutput+6, &uid, 4);
	}
	void unpacket(char *pInput)
	{
		memcpy(&length, pInput, 2);
		memcpy(&type, pInput+2, 2);
		memcpy(&command, pInput+4, 2);
		memcpy(&uid, pInput+6, 4);
	}

	//获取redis key
	string getKey()
	{
		string key = "user.";
		stringstream keyss;
		keyss << uid;
		key += keyss.str();
		return key;
	}
	//获取redis key
	static string getKey(unsigned int userid)
	{
		string key = "user.";
		stringstream keyss;
		keyss << userid;
		key += keyss.str();
		return key;
	}
}pkg_head;
#pragma pack()

#endif

