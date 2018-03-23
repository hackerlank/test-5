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

//redis 连接客户端
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
	//设置过期时间
	void setExpire(const string key,unsigned int seconds);
	// Key
	bool exists(string key);
	//查找所有符合给定模式 pattern 的 key
	bool keys(const string pattern,list<string>& result);
	// String
    void set(const string key, const string value);
	string get(const string key);
	// Hash
	//为哈希表 key 中的域 field 的值加上增量 value
	//增量也可以为负数，相当于对给定域进行减法操作
	int hincrby(const string key, const string field, const string value);
	//hash中是否存在该域
	bool hexists(const string key, const string field);
	//将哈希表 key 中的域 field 的值设为 value
	int hset(const string key, const string field, const string value);
	//取Hash 单个值
	string hget(const string key, const string field);
	//返回哈希表 key 中，所有的域和值。
	bool hgetall(const string key, map<string, string>& result);
	//设置Hash 多个值
	int hmset(const string key, const string field_value);
	//取Hash 多个值
	bool hmget(const string key, const string field,list<string>& result);
	// List
	//将一个或多个值 value 插入到列表 key 的表头
	int lpush(const string key,const string value);
	//将一个或多个值 value 插入到列表 key 的表尾(最右边)。
	int rpush(const string key,const string value);
	//移除并返回列表 key 的尾元素。
	string rpop(const string key);
	//返回列表 key 的长度。如果 key 不存在，则 key 被解释为一个空列表，返回 0 .
	int llen(const string key);
	// Sorted Set
	//将一个或多个 member 元素及其 score 值加入到有序集 key 当中
	int zadd(const string key,const string score_member);
	//返回有序集 key 中，指定区间内的成员。其中成员的位置按 score 值递增(从小到大)来排序。
	bool zrange(const string key,const int start,const int stop,vector<string>& result,bool ifscore = false);
	//返回有序集 key 中成员 member 的排名。其中有序集成员按 score 值递增(从小到大)顺序排列。
	//排名以 0 为底，也就是说， score 值最小的成员排名为 0 。
	int zrank(const string key,const string member);
	//返回有序集 key 中成员 member 的排名。其中有序集成员按 score 值递减(从大到小)排序。
	//排名以 0 为底，也就是说， score 值最小的成员排名为 0 。
	int zrevrank(const string key,const string member);
	//返回有序集 key 的基数。当 key 不存在时，返回 0 。
	int zcard(const string key);
	//计算给定的一个或多个有序集的交集，其中给定 key 的数量必须以 numkeys 参数指定，并将该交集(结果集)储存到 destination 。
	//默认情况下，结果集中某个成员的 score 值是所有给定集下该成员 score 值之和.
	int zinterstore(const string destkey,list<string>& srckeys);
	//返回有序集 key 中，成员 member 的 score 值。
	//如果 member 元素不是有序集 key 的成员，或 key 不存在，返回 nil 。
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
//连接
class RedisConnection
{
	public:
		//当前时间
		unsigned int m_nowtime;
	public:
		RedisConnection(RedisPool* redisPool);
		~RedisConnection();
		//获取reids上下文
		redisContext* getRedisContext()
		{
			return redisContext_;
		}
		//连接
		int connect();
		//检查返回值
		bool checkReply(const redisReply *reply);
		//设置过期时间
		void setExpire(const string key,unsigned int seconds);
		//ping 服务器
		bool ping();
		//查找所有符合给定模式 pattern 的 key
		bool keys(const string pattern,list<string>& result);
		//判断key值是否存在
		bool exists(const string key);
		/////////////////////////////////// String //////////////////////////////////////////////
		//设置普通的value
	   	bool set(const string key, const string value);
		//获取普通的value
	    	string get(const string key);
		/////////////////////////////////// Hash ///////////////////////////////////////////////
		//为哈希表 key 中的域 field 的值加上增量 value
		//增量也可以为负数，相当于对给定域进行减法操作
		int hincrby(const string key, const string field, const string value);
		//hash中是否存在该域
		bool hexists(const string key, const string field);
		//设置Hash 单个值
		int hset(const string key, const string field, const string value);
		//取Hash 单个值
		string hget(const string key, const string field);
		//返回哈希表 key 中，所有的域和值。
		bool hgetall(const string key, map<string, string>& result);
		//设置Hash 多个值
		int hmset(const string key, const string field_value);
		//取Hash 多个值
		bool hmget(const string key, const string field,list<string>& result);
		/////////////////////////////////// List ///////////////////////////////////////////////
		//将一个或多个值 value 插入到列表 key 的表头
		/*
			如果有多个 value 值，那么各个 value 值按从左到右的顺序依次插入到表头： 比如说，对空列表 mylist 执行命令 LPUSH mylist a b c ，列表的值将是 c b a ，这等同于原子性地执行 LPUSH mylist a 、 LPUSH mylist b 和 LPUSH mylist c 三个命令。

			如果 key 不存在，一个空列表会被创建并执行 LPUSH 操作。

			当 key 存在但不是列表类型时，返回一个错误。
		*/
		int lpush(const string key,const string value);
		//将一个或多个值 value 插入到列表 key 的表尾(最右边)。
		int rpush(const string key,const string value);
		//移除并返回列表 key 的尾元素。
		string rpop(const string key);
		//返回列表 key 的长度。如果 key 不存在，则 key 被解释为一个空列表，返回 0 .
		int llen(const string key);
		//将一个或多个 member 元素及其 score 值加入到有序集 key 当中
		int zadd(const string key,const string score_member);
		//返回有序集 key 中，指定区间内的成员。其中成员的位置按 score 值递增(从小到大)来排序。
		bool zrange(const string key,const int start,const int stop,vector<string>& result,bool ifscore = false);
		//返回有序集 key 中成员 member 的排名。其中有序集成员按 score 值递增(从小到大)顺序排列。
		//排名以 0 为底，也就是说， score 值最小的成员排名为 0 。
		int zrank(const string key,const string member);
		//返回有序集 key 中成员 member 的排名。其中有序集成员按 score 值递减(从大到小)排序。
		int zrevrank(const string key,const string member);
		//返回有序集 key 的基数。当 key 不存在时，返回 0 。
		int zcard(const string key);
		//计算给定的一个或多个有序集的交集
		int zinterstore(const string destkey,list<string>& srckeys);
		//返回有序集 key 中，成员 member 的 score 值。
		//如果 member 元素不是有序集 key 的成员，或 key 不存在，返回 nil 。
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
	bool ifneedauth;//是否需要验证密码
	string authpwd;
	string name_;

};


#endif	/* REDISCLIENT_H */



