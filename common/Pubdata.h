#ifndef __PUBDATA_H__
/*
desc:���õ����ݽṹ
writer:Lake
*/
#define __PUBDATA_H__

#include <sstream>
#include <string.h>

#define	INNER_CONNECT			1	//����
#define	OUT_CONNECT				2	//����
#define PROCESS_TIME			3	//��ʱ

#define LEN_PKG_HEAD 			10  //��ͷ����
#define TYPE_LOBBY				10	//��������type
#define	MAX_GW_COUNT			10	//�����������
#define TYPE_GW					1	//������������
#define	COMMAND_GW_REGISTER		1
#define COMMAND_GW_KICKUSER		3
#define COMMAND_ROOM_REGISTER   1



#define LEN_BUFF_DATA_50k	10240*5		//50k���ݳ���
#define LEN_BUFF_DATA_40k	10240*4		//40k���ݳ���
#define LEN_BUFF_DATA_30k	10240*3		//30k���ݳ���
#define LEN_BUFF_DATA_20k	10240*2		//20k���ݳ���
#define LEN_BUFF_DATA_10k	10240		//10k���ݳ���
#define LEN_BUFF_DATA_4k	4096		//4k���ݳ���
#define LEN_BUFF_DATA_2k	2048		//2k���ݳ���
#define LEN_BUFF_DATA_1k	1024		//1k���ݳ���
#define LEN_BUFF_DATA_512	512			//512byte���ݳ���
#define LEN_BUFF_DATA_256	256			//256byte���ݳ���
#define LEN_BUFF_DATA_128	128			//128byte���ݳ���
#define LEN_BUFF_DATA_64	64			//64byte���ݳ���
#define LEN_BUFF_DATA_32	32			//32byte���ݳ���
#define LEN_BUFF_DATA_16	16			//16byte���ݳ���
#define DATE_ONE_DAY_SECS	86400		//һ�������
#define DATE_ONE_HOUR_SECS	3600		//һСʱ������
#define DATE_FIVE_MIN_SECS	300 		//5��������

#define MAX_AI_NPC_UID		10000		//npc�û����id	

#define	LEN_SINGLE_MSG		8192		// ������Ϣ����󳤶�
//�����������
#define	COMMAND_USER_LOGIN				1		//�û���¼
#define	COMMAND_USER_OFFLINE			2		//�û�����
#define	COMMAND_GAME_USER_CHAT			33		//��������
#define	COMMAND_GAME_SHOWCHAT			900		//���黥������
#define COMMAND_GAME_ERROR				888 	//��Ϸ����
#define COMMAND_LOBBY_GET_COIN			2		//��ȡ��Ϸ��
#define	COMMAND_LOBBY_JOIN_ROOM			10		//���뷿��
#define	COMMAND_LOBBY_LEFT_ROOM			11		//�뿪����
#define	COMMAND_LOBBY_GAME_CHONGZHI		20		//��ֵ
#define	COMMAND_LOBBY_GAME_GET_COIN		21		//��Ϸ�и�����Ϸ��
#define	COMMAND_LOBBY_GAME_USER_LEFT	22		//��Ϸ���û��뿪
#define	COMMAND_LOBBY_GAME_SETTLE		23		//��Ϸ����

#define	COMMAND_LOBBY_GAME_UPDATERWRATE	500		//���·��������Ӯ����




//������Ϣ����
#define ERROR_REDIS_DATA_NULL			100 //��������Ϊ��
#define ERROR_COMMON_SERVER   			101  //�������ڲ�����
#define ERROR_COMMON_REQUEST   			102  //�����������
#define ERROR_COMMON_COIN_LESS   		103  //�𶹲���
#define ERROR_COMMON_INGOT_LESS   		104  //Ԫ������
#define ERROR_COMMON_INTEGRAL_LESS   	105  //���ֲ���
#define ERROR_LOBBY_ROOMNUM_MAX   		106  //������������
#define ERROR_LOBBY_ROOM_IN_GAME		107  //���û�ԭ�����ڵķ�����Ϸû�н���
#define ERROR_COIN_LESS					108 //�˺�����
#define ERROR_DESK_NO_EXIST				109 //���Ӳ�����
#define ERROR_DESK_NO_USER				110 //�û�������
#define ERROR_REQUEST_PARAM				111 //�����������
#define ERROR_COMMON_COIN_MORE   		112  //�𶹳�����������
#define ERROR_COMMON_INTEGRAL_MORE   	113  //���ֳ�����������
#define ERROR_COMMON_ISGAMING   		115  //������Ϸ��
#define ERROR_COMMON_POS_NO_EXIST   	116  //λ�ò�����
#define ERROR_COMMON_NO_POS   			117  //û�п����λ�ÿ�������
#define ERROR_LOBBY_NO_ROOM				118 //�������䲻����
#define ERROR_MATCH_START				150 //�����Ѿ���ʼ
#define ERROR_MATCH_END					151 //�����Ѿ�����
#define ERROR_MATCH_EXIST				152 //����������
#define ERROR_MATCH_IN					153 //���ڱ�����
#define ERROR_MATCH_NO_APPLY			154 //û�б�������
#define ERROR_MATCH_NO_START			155 //����û�п�ʼ
#define ERROR_MATCH_APPLY_ALY			156 //�Ѿ�������ǰ����
#define ERROR_SELFROOM_CODE				170 //������������
#define ERROR_SELFROOM_OUTOFDATE		171 //�����Ѿ�����
#define ERROR_SELFROOM_NOSEFL			172 //ֻ�з������ܲ���


//���ֽ�
#define ReadInt8(data,buf,buflen)      memcpy(data,buf,1); buflen += 1;
#define ReadInt16(data,buf,buflen)     memcpy(data,buf,2); buflen += 2;
#define ReadInt32(data,buf,buflen)     memcpy(data,buf,4); buflen += 4;
#define ReadInt64(data,buf,buflen)	    memcpy(data,buf,8); buflen += 8;
//д�ֽ�
#define WriteInt8(buf,data,lenvalue)    memcpy(buf,data,1); buf += 1; lenvalue += 1;
#define WriteInt16(buf,data,lenvalue)  memcpy(buf,data,2); buf += 2; lenvalue += 2;
#define WriteInt32(buf,data,lenvalue)  memcpy(buf,data,4); buf += 4; lenvalue += 4;
#define WriteInt64(buf,data,lenvalue)  memcpy(buf,data,8); buf += 8; lenvalue += 8;

using namespace std;

//��������
enum CoinType
{
	CoinType_Coin = 1,//���
	CoinType_Integral//����
};
//����Ƶ������
enum ChatType
{
	Chat_Sys = 1,
	Chat_World,
	Chat_Private
};
//���ñ仯log����
enum LogType
{
	LogType_Win = 1,//Ӯ
	LogType_Lose = 2,//��
	LogType_Basefee = 3,//̨��
	LogType_Matchfee = 4,//����������
	LogType_MatchAward=5,//������������
	LogType_SHOPBUY=6,//�̳ǹ�����
	LogType_MailAward=7,//�ʼ���ȡ����
	LogType_ZJHBET =8,//ը��Ѻע
	LogType_UseMagicFace=9,//ʹ��ħ������
	LogType_TEXASBET =10,//����Ѻע
	LogType_TEXASIN =11,//����Я��������
	LogType_TEXASOUT =12,//����Я������˳�
	LogType_TEXASGIRL =13,//���ݴ��ͺɹ�
	LogType_CHESSBET =14,//������ע
	LogType_CHESSOUT =15,//���巵��
	LogType_MJ_Gang_Win = 16,//�齫��Ӯ
	LogType_MJ_Gang_Lose = 17,//�齫����
	LogType_MJ_HU_Win = 18,//�齫��Ӯ
	LogType_MJ_HU_Lose = 19,//�齫����
	LogType_MJ_HuaPig_Win = 20,//�齫����Ӯ
	LogType_MJ_HuaPig_Lose = 21,//�齫������
	LogType_MJ_NoTing_Win = 22,//�齫����Ӯ
	LogType_MJ_NoTing_Lose = 23,//�齫������
	LogType_MJ_WindRain_Win = 24,//�齫�η����귵��
	LogType_MJ_WindRain_Lose = 25,//�齫�η������˻�
	LogType_ROBOTADD =70,//�����˲�����
	LogType_ROBOTRED =71,//�����˻��ս��
	LogType_CHONGZHI=88	//��ֵ
};
//��Ϸ����
enum GameType
{
	GameType_Ddz = 1,//������
	GameType_Zjh = 2,//ը��
	GameType_Nn = 3,//ţţ
	GameType_Texas = 4,//�����˿�
	GameType_SCMJ = 5,//�Ĵ��齫
	GameType_Chess = 6//�й�����
};
//����ȼ�����
enum RoomLvType
{
	RoomLvType_lV1 = 1,//����
	RoomLvType_lV2 = 2,//�м�
	RoomLvType_lV3 = 3,//�߼�
	RoomLvType_lV4 = 4//����
};

//����������Ϣ
struct MatchAwardInfo
{
	unsigned int gametype;//��Ϸ����
	unsigned int roomlevel;//����ȼ� 1 ���� 2 �м� 3 �߼�...
	unsigned int lowrank;//��������
	unsigned int highrank;//��������
	unsigned int coin;//������
	
	MatchAwardInfo(){
		gametype = 0;
		roomlevel = 0;
		highrank = 0;
		lowrank = 0;
		coin = 0;
	}
};
//��Ʒ��Ϣ
struct GoodsInfo
{
	unsigned int id;//��Ʒid
	unsigned int price;//�۸�RMB
	unsigned int coin;//��ý�
	unsigned int bonus;//�����ý�
	GoodsInfo()
	{
		id = 0;
		price = 0;
		coin = 0;
		bonus = 0;
	}
};
//�Ƹ�����Ϣ
struct CardProbInfo
{
	unsigned int cardid;//��id
	unsigned int normalprob;//һ�����
	unsigned int goodprob;//�õĸ���
	unsigned int badprob;//���ĸ���
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
	unsigned short		length;//����
	unsigned short		type;//����
	unsigned short		command;//����
	unsigned int		uid;//�û�id

	unsigned int 		nowtime;//��ǰʱ��
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

	//��ȡredis key
	string getKey()
	{
		string key = "user.";
		stringstream keyss;
		keyss << uid;
		key += keyss.str();
		return key;
	}
	//��ȡredis key
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

