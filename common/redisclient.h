/*
 */
#ifndef REDISCLIENT_H
#define	REDISCLIENT_H

#include "hiredis.h"
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <exception>
#include <list>
#include <vector>
#include <map>
//#include <new>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <stdint.h>
#include <string.h>
//#include <thread>
#include <functional>
//#include <chrono>
#include <time.h>
using namespace std;

class RedisPool;

//redis ���ӿͻ���
class RedisClient
{
public:
	RedisClient(const string ip,
					uint16_t port,
					int minConn = 1,
					int maxConn = 8,
                   			int dbNo = 0,
					const string nameArg = string(),bool ifneedauth=false,const string authpwd = string());
	~RedisClient();

	int init();

	// Connection
	//���ù���ʱ��
	void setExpire(const string key,unsigned int seconds);
	// Key
	bool exists(string key);
	//�������з��ϸ���ģʽ pattern �� key
	bool keys(const string pattern,list<string>& result);
	// String
    void set(const string key, const string value);
	string get(const string key);
	// Hash
	//Ϊ��ϣ�� key �е��� field ��ֵ�������� value
	//����Ҳ����Ϊ�������൱�ڶԸ�������м�������
	int hincrby(const string key, const string field, const string value);
	//hash���Ƿ���ڸ���
	bool hexists(const string key, const string field);
	//����ϣ�� key �е��� field ��ֵ��Ϊ value
	int hset(const string key, const string field, const string value);
	//ȡHash ����ֵ
	string hget(const string key, const string field);
	//���ع�ϣ�� key �У����е����ֵ��
	bool hgetall(const string key, map<string, string>& result);
	//����Hash ���ֵ
	int hmset(const string key, const string field_value);
	//ȡHash ���ֵ
	bool hmget(const string key, const string field,list<string>& result);
	// List
	//��һ������ֵ value ���뵽�б� key �ı�ͷ
	int lpush(const string key,const string value);
	//��һ������ֵ value ���뵽�б� key �ı�β(���ұ�)��
	int rpush(const string key,const string value);
	//�Ƴ��������б� key ��βԪ�ء�
	string rpop(const string key);
	//�����б� key �ĳ��ȡ���� key �����ڣ��� key ������Ϊһ�����б����� 0 .
	int llen(const string key);
	// Sorted Set
	//��һ������ member Ԫ�ؼ��� score ֵ���뵽���� key ����
	int zadd(const string key,const string score_member);
	//�������� key �У�ָ�������ڵĳ�Ա�����г�Ա��λ�ð� score ֵ����(��С����)������
	bool zrange(const string key,const int start,const int stop,vector<string>& result,bool ifscore = false);
	//�������� key �г�Ա member ���������������򼯳�Ա�� score ֵ����(��С����)˳�����С�
	//������ 0 Ϊ�ף�Ҳ����˵�� score ֵ��С�ĳ�Ա����Ϊ 0 ��
	int zrank(const string key,const string member);
	//�������� key �г�Ա member ���������������򼯳�Ա�� score ֵ�ݼ�(�Ӵ�С)����
	//������ 0 Ϊ�ף�Ҳ����˵�� score ֵ��С�ĳ�Ա����Ϊ 0 ��
	int zrevrank(const string key,const string member);
	//�������� key �Ļ������� key ������ʱ������ 0 ��
	int zcard(const string key);
	//���������һ���������򼯵Ľ��������и��� key ������������ numkeys ����ָ���������ý���(�����)���浽 destination ��
	//Ĭ������£��������ĳ����Ա�� score ֵ�����и������¸ó�Ա score ֵ֮��.
	int zinterstore(const string destkey,list<string>& srckeys);
	//�������� key �У���Ա member �� score ֵ��
	//��� member Ԫ�ز������� key �ĳ�Ա���� key �����ڣ����� nil ��
	string zscore(const string key,const string member);

private:
	const string hostip_;
	uint16_t hostport_;
	int minConn_;
	int maxConn_;
	int dbNo_;
	const string name_;
	RedisPool* redisPool_;
};
//����
class RedisConnection
{
	public:
		//��ǰʱ��
		unsigned int m_nowtime;
	public:
		RedisConnection(RedisPool* redisPool);
		~RedisConnection();
		//��ȡreids������
		redisContext* getRedisContext()
		{
			return redisContext_;
		}
		//����
		int connect();
		//��鷵��ֵ
		bool checkReply(const redisReply *reply);
		//���ù���ʱ��
		void setExpire(const string key,unsigned int seconds);
		//ping ������
		bool ping();
		//�������з��ϸ���ģʽ pattern �� key
		bool keys(const string pattern,list<string>& result);
		//�ж�keyֵ�Ƿ����
		bool exists(const string key);
		/////////////////////////////////// String //////////////////////////////////////////////
		//������ͨ��value
	   	bool set(const string key, const string value);
		//��ȡ��ͨ��value
	    	string get(const string key);
		/////////////////////////////////// Hash ///////////////////////////////////////////////
		//Ϊ��ϣ�� key �е��� field ��ֵ�������� value
		//����Ҳ����Ϊ�������൱�ڶԸ�������м�������
		int hincrby(const string key, const string field, const string value);
		//hash���Ƿ���ڸ���
		bool hexists(const string key, const string field);
		//����Hash ����ֵ
		int hset(const string key, const string field, const string value);
		//ȡHash ����ֵ
		string hget(const string key, const string field);
		//���ع�ϣ�� key �У����е����ֵ��
		bool hgetall(const string key, map<string, string>& result);
		//����Hash ���ֵ
		int hmset(const string key, const string field_value);
		//ȡHash ���ֵ
		bool hmget(const string key, const string field,list<string>& result);
		/////////////////////////////////// List ///////////////////////////////////////////////
		//��һ������ֵ value ���뵽�б� key �ı�ͷ
		/*
			����ж�� value ֵ����ô���� value ֵ�������ҵ�˳�����β��뵽��ͷ�� ����˵���Կ��б� mylist ִ������ LPUSH mylist a b c ���б��ֵ���� c b a �����ͬ��ԭ���Ե�ִ�� LPUSH mylist a �� LPUSH mylist b �� LPUSH mylist c �������

			��� key �����ڣ�һ�����б�ᱻ������ִ�� LPUSH ������

			�� key ���ڵ������б�����ʱ������һ������
		*/
		int lpush(const string key,const string value);
		//��һ������ֵ value ���뵽�б� key �ı�β(���ұ�)��
		int rpush(const string key,const string value);
		//�Ƴ��������б� key ��βԪ�ء�
		string rpop(const string key);
		//�����б� key �ĳ��ȡ���� key �����ڣ��� key ������Ϊһ�����б����� 0 .
		int llen(const string key);
		//��һ������ member Ԫ�ؼ��� score ֵ���뵽���� key ����
		int zadd(const string key,const string score_member);
		//�������� key �У�ָ�������ڵĳ�Ա�����г�Ա��λ�ð� score ֵ����(��С����)������
		bool zrange(const string key,const int start,const int stop,vector<string>& result,bool ifscore = false);
		//�������� key �г�Ա member ���������������򼯳�Ա�� score ֵ����(��С����)˳�����С�
		//������ 0 Ϊ�ף�Ҳ����˵�� score ֵ��С�ĳ�Ա����Ϊ 0 ��
		int zrank(const string key,const string member);
		//�������� key �г�Ա member ���������������򼯳�Ա�� score ֵ�ݼ�(�Ӵ�С)����
		int zrevrank(const string key,const string member);
		//�������� key �Ļ������� key ������ʱ������ 0 ��
		int zcard(const string key);
		//���������һ���������򼯵Ľ���
		int zinterstore(const string destkey,list<string>& srckeys);
		//�������� key �У���Ա member �� score ֵ��
		//��� member Ԫ�ز������� key �ĳ�Ա���� key �����ڣ����� nil ��
		string zscore(const string key,const string member);
	private:
		redisContext* 	redisContext_;
		uint64_t lastActiveTime_;
		RedisPool* redisPool_;
};

class RedisPool
{
public:
	RedisPool(const string ip,
					uint16_t port,
					int minConn,
	  				int maxConn,
					int dbNo,
					const string nameArg,bool ifneedauth=false,const string authpwd = string());
	~RedisPool();

	int init();
	void serverCron();

	int getDBNo() { return dbNo_; }

	const char* getServerIP() { return hostip_.c_str(); }
	int getServerPort() { return hostport_; }

    RedisConnection* getConnection();
    void freeConnection(RedisConnection*);
private:
	string hostip_;
	int hostport_;
	int minConn_;
	int maxConn_;
	int dbNo_;
	list<RedisConnection*>	connections_;
	bool quit_;
	pthread_mutex_t m_lock_cons;
public:
	bool ifneedauth;//�Ƿ���Ҫ��֤����
	string authpwd;
	string name_;

};


#endif	/* REDISCLIENT_H */



