#include "redisclient.h"
#include <execinfo.h>
#include <stdlib.h>
#include <sstream>

namespace
{
	string Int_to_String(int n)
	{
		ostringstream stream;
		stream<<n;  //n为int类型
		return stream.str();
	}
}

////////////////////////RedisClient///////////////////////////////////////////////////////////

RedisClient::RedisClient(const string ip,
					uint16_t port,
					int minConn,
					int maxConn,
					int dbNo,
					const string nameArg,bool ifneedauth,const string authpwd)
	: hostip_(ip),
	  hostport_(port),
	  minConn_(minConn),
	  maxConn_(maxConn),
	  dbNo_(dbNo),
	  name_(nameArg),
	  redisPool_(new RedisPool(ip, port, minConn,maxConn,dbNo,nameArg,ifneedauth,authpwd))
{
	redisPool_->init();
}


RedisClient::~RedisClient()
{
	if (redisPool_)
	{
		delete redisPool_;
		redisPool_ = NULL;
	}
}
//设置过期时间
void RedisClient::setExpire(const string key,unsigned int seconds)
{
	if(key.length()==0)
		return;
	RedisConnection* conn = redisPool_->getConnection();
	conn->setExpire(key, seconds);
	redisPool_->freeConnection(conn);
}
//查找所有符合给定模式 pattern 的 key
bool RedisClient::keys(const string pattern,list<string>& result)
{
	if(pattern.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool res = conn->keys(pattern,result);
	redisPool_->freeConnection(conn);
	return res;
}
//key值是否存在
bool RedisClient::exists(const string key)
{
	if(key.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool result = conn->exists(key);
	redisPool_->freeConnection(conn);
	return result;
}
//设置key value
void RedisClient::set(const string key, const string value)
{
	if(key.length()==0)
		return;
	RedisConnection* conn = redisPool_->getConnection();
	conn->set(key, value);
	redisPool_->freeConnection(conn);
}
//为哈希表 key 中的域 field 的值加上增量 value
//增量也可以为负数，相当于对给定域进行减法操作
int RedisClient::hincrby(const string key, const string field, const string value)
{
	if(key.length()==0 || field.length()==0 || value.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->hincrby(key, field, value);
	redisPool_->freeConnection(conn);
	return result;
}
//获取value
string RedisClient::get(const string key)
{
	if(key.length()==0)
		return "";
	RedisConnection* conn = redisPool_->getConnection();
	string value = conn->get(key);
	redisPool_->freeConnection(conn);
	return value;
}
//hash中是否存在该域
bool RedisClient::hexists(const string key, const string field)
{
	if(key.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool result = conn->hexists(key, field);
	redisPool_->freeConnection(conn);
	return result;
}
//设置hash key value 单个值
int RedisClient::hset(const string key, const string field, const string value)
{
	if(key.length()==0 || field.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->hset(key, field, value);
	redisPool_->freeConnection(conn);
	return result;
}
//取Hash 单个值
string RedisClient::hget(const string key, const string field)
{
	if(key.length()==0 || field.length()==0)
		return "";
	RedisConnection* conn = redisPool_->getConnection();
	string result = conn->hget(key, field);
	redisPool_->freeConnection(conn);
	return result;
}
//设置Hash 多个值
int RedisClient::hmset(const string key, const string field_value)
{
	if(key.length()==0 || field_value.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->hmset(key, field_value);
	redisPool_->freeConnection(conn);
	return result;
}
//取Hash 多个值
bool RedisClient::hmget(const string key, const string field,list<string>& result)
{
	if(key.length()==0 || field.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->hmget(key, field,result);
	redisPool_->freeConnection(conn);
	return ret;
}
//返回哈希表 key 中，所有的域和值
bool RedisClient::hgetall(const string key, map<string, string>& result)
{
	if(key.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->hgetall(key, result);
	redisPool_->freeConnection(conn);
	return ret;
}
//将一个或多个值 value 插入到列表 key 的表头
int RedisClient::lpush(const string key,const string value)
{
	if(key.length()==0 || value.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->lpush(key, value);
	redisPool_->freeConnection(conn);
	return ret;
}
//将一个或多个值 value 插入到列表 key 的表尾
int RedisClient::rpush(const string key,const string value)
{
	if(key.length()==0 || value.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->rpush(key, value);
	redisPool_->freeConnection(conn);
	return ret;
}

//移除并返回列表 key 的尾元素。
string RedisClient::rpop(const string key)
{
	if(key.length()==0)
		return "";
	RedisConnection* conn = redisPool_->getConnection();
	string result = conn->rpop(key);
	redisPool_->freeConnection(conn);
	return result;
}
//返回列表 key 的长度。如果 key 不存在，则 key 被解释为一个空列表，返回 0 .
int RedisClient::llen(const string key)
{
	if(key.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->llen(key);
	redisPool_->freeConnection(conn);
	return result;
}
//将一个或多个 member 元素及其 score 值加入到有序集 key 当中
int RedisClient::zadd(const string key,const string score_member)
{
	if(key.length()==0 || score_member.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zadd(key, score_member);
	redisPool_->freeConnection(conn);
	return result;
}
//返回有序集 key 中，指定区间内的成员。其中成员的位置按 score 值递增(从小到大)来排序。
bool RedisClient::zrange(const string key,const int start,const int stop,vector<string>& result,bool ifscore)
{
	if(key.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->zrange(key, start,stop,result,ifscore);
	redisPool_->freeConnection(conn);
	return ret;
}
//返回有序集 key 中成员 member 的排名。其中有序集成员按 score 值递增(从小到大)顺序排列。
//排名以 0 为底，也就是说， score 值最小的成员排名为 0 。
int RedisClient::zrank(const string key,const string member)
{
	if(key.length()==0 || member.length()==0)
		return -1;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zrank(key, member);
	redisPool_->freeConnection(conn);
	return result;
}
//返回有序集 key 中成员 member 的排名。其中有序集成员按 score 值递减(从大到小)排序。
//排名以 0 为底，也就是说， score 值最小的成员排名为 0 。
int RedisClient::zrevrank(const string key,const string member)
{
	if(key.length()==0 || member.length()==0)
		return -1;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zrevrank(key, member);
	redisPool_->freeConnection(conn);
	return result;
}
//返回有序集 key 中，成员 member 的 score 值。
//如果 member 元素不是有序集 key 的成员，或 key 不存在，返回 nil 。
string RedisClient::zscore(const string key,const string member)
{
	if(key.length()==0 || member.length()==0)
		return "";
	RedisConnection* conn = redisPool_->getConnection();
	string result = conn->zscore(key, member);
	redisPool_->freeConnection(conn);
	return result;
}

//返回有序集 key 的基数。当 key 不存在时，返回 0 。
int RedisClient::zcard(const string key)
{
	if(key.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zcard(key);
	redisPool_->freeConnection(conn);
	return result;
}
//计算给定的一个或多个有序集的交集
int RedisClient::zinterstore(const string destkey,list<string>& srckeys)
{
	if(destkey.length()==0 && srckeys.size()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zinterstore(destkey,srckeys);
	redisPool_->freeConnection(conn);
	return result;
}
////////////////////////RedisConnection///////////////////////////////////////////////////////////

RedisConnection::RedisConnection(RedisPool* redisPool)
	: redisContext_(NULL),
	  lastActiveTime_(time(NULL)),
	  redisPool_(redisPool)
{
	m_nowtime = time(NULL);
}
RedisConnection::~RedisConnection()
{
	if (redisContext_)
	{
		redisFree(redisContext_);
		redisContext_ = NULL;
	}
}
//连接redis
int RedisConnection::connect()
{
	if (redisContext_)
	{
		redisFree(redisContext_);
		redisContext_ = NULL;
	}
	struct timeval timeout = {0, 1000000}; // 1s
	redisContext_ = redisConnectWithTimeout(redisPool_->getServerIP(), redisPool_->getServerPort(), timeout);
	if (!redisContext_ || redisContext_->err)
	{
		if (redisContext_)
		{
			redisFree(redisContext_);
			redisContext_ = NULL;
		}
		printf("connect redis error--[%s][%s::%d]\n",redisPool_->name_.c_str(),redisPool_->getServerIP(), redisPool_->getServerPort());
		return -1;
	}else{
		printf("connect redis success--[%s][%s::%d]\n",redisPool_->name_.c_str(),redisPool_->getServerIP(), redisPool_->getServerPort());
	}
	redisReply* reply = NULL;
	if(redisPool_->getDBNo()>0){
		reply = static_cast<redisReply*>(redisCommand(redisContext_, "SELECT %d", redisPool_->getDBNo()));
		if (!checkReply(reply))
		{
			printf("SELECT redis DB error--[%s][%s::%d][db=%d]\n",redisPool_->name_.c_str(),redisPool_->getServerIP(), redisPool_->getServerPort(),redisPool_->getDBNo());
		}else{
			printf("SELECT redis DB success--[%s][%s::%d][db=%d]\n",redisPool_->name_.c_str(),redisPool_->getServerIP(), redisPool_->getServerPort(),redisPool_->getDBNo());
		}
		if(reply != NULL)
			freeReplyObject(reply);
	}
	if(redisPool_->ifneedauth){
		reply= static_cast<redisReply*>(redisCommand(redisContext_, "AUTH %s", redisPool_->authpwd.c_str()));
		if (reply->type == REDIS_REPLY_ERROR) {
		   	printf("AUTH redis error--[%s][%s::%d--AUTH %s]\n",redisPool_->name_.c_str(),redisPool_->getServerIP(), redisPool_->getServerPort(),redisPool_->authpwd.c_str());
		}else{
			printf("AUTH redis success--[%s][%s::%d--AUTH %s]\n",redisPool_->name_.c_str(),redisPool_->getServerIP(), redisPool_->getServerPort(),redisPool_->authpwd.c_str());
		}
		if(reply != NULL)
			freeReplyObject(reply);
	}
	return 0;
}
//检查返回值
bool RedisConnection::checkReply(const redisReply* reply)
{
	if(reply == NULL)
		return false;

	switch(reply->type)
	{
		case REDIS_REPLY_STRING:
			return true;
		case REDIS_REPLY_ARRAY:
			return true;
		case REDIS_REPLY_INTEGER:
			return true;
		case REDIS_REPLY_NIL:
			return false;
		case REDIS_REPLY_STATUS:
			return (strcasecmp(reply->str, "OK") == 0) ? true : false;
		case REDIS_REPLY_ERROR:
			return false;
		default:
			return false;
	}

	return false;
}
//设置过期时间
void RedisConnection::setExpire(const string key,unsigned int seconds)
{
	if(key.length()==0)
		return;
	if(redisContext_ == NULL)
		return ;
	redisReply* reply = (redisReply*)(redisCommand(redisContext_, "EXPIRE %s %d", key.c_str(), seconds));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return;
	}

    freeReplyObject(reply);
}

//ping 是否连接还有效
bool RedisConnection::ping()
{
	if(redisContext_ == NULL)
		return false;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "PING"));
	if (reply == NULL)
		return false;

	freeReplyObject(reply);
	return true;
}
//查找所有符合给定模式 pattern 的 key
bool RedisConnection::keys(const string pattern,list<string>& result)
{
	if(pattern.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "KEYS %s", pattern.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}

	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (unsigned int i = 0; i < reply->elements; i ++)
		{
			string value(reply->element[i]->str,reply->element[i]->len);
			result.push_back(value);
		}
	}
	freeReplyObject(reply);
	return true;
}
//是否存在key
bool RedisConnection::exists(const string key)
{
	if(key.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	int result;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "EXISTS %s", key.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}

	result = reply->integer;
	freeReplyObject(reply);
	return result == 1;
}
//设置key value
bool RedisConnection::set(const string key, const string value)
{
	if(key.length()==0 || value.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
    redisReply* reply = (redisReply*)(redisCommand(redisContext_, "SET %s %s", key.c_str(), value.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}

    freeReplyObject(reply);

    return true;
}
//获取value
string RedisConnection::get(const string key)
{
	if(key.length()==0)
		return "";
	if(redisContext_ == NULL)
		return "";
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "GET %s", key.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return "";
	}

	string result;
	if (reply->type == REDIS_REPLY_STRING)
		result.append(reply->str, reply->len);

	freeReplyObject(reply);
	return result;
}
//为哈希表 key 中的域 field 的值加上增量 value
//增量也可以为负数，相当于对给定域进行减法操作
int RedisConnection::hincrby(const string key, const string field, const string value)
{
	if(key.length()==0 || field.length()==0 || value.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,"HINCRBY %s %s %s", key.c_str(), field.c_str(), value.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}
//hash中是否存在该域
bool RedisConnection::hexists(const string key, const string field)
{
	if(key.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	int result;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "HEXISTS %s %s", key.c_str(), field.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}

	result = reply->integer;
	freeReplyObject(reply);
	return result == 1;
}
//设置hash 一个值
int RedisConnection::hset(const string key, const string field, const string value)
{
	if(key.length()==0 || field.length()==0 || value.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,"HSET %s %s %s", key.c_str(), field.c_str(), value.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}
//取Hash 单个值
string RedisConnection::hget(const string key, const string field)
{
	if(key.length()==0 || field.length()==0)
		return "";
	if(redisContext_ == NULL)
		return "";
	string result;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,"HGET %s %s", key.c_str(), field.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return "";
	}

	result.append(reply->str, reply->len);
	freeReplyObject(reply);
	return result;
}
//设置Hash 多个值
int RedisConnection::hmset(const string key, const string field_value)
{
	if(key.length()==0 || field_value.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	string chstr = "HMSET " + key + " " + field_value;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}
//取Hash 多个值
bool RedisConnection::hmget(const string key, const string field,list<string>& result)
{
	if(key.length()==0 || field.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	string chstr = "HMGET " + key + " " + field;
	//printf("check_str = %s\n",chstr.c_str());
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}

	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < reply->elements; i ++)
		{
			string value(reply->element[i]->str,reply->element[i]->len);
			result.push_back(value);
		}
	}
	freeReplyObject(reply);
	return true;
}
//返回哈希表 key 中，所有的域和值。
bool RedisConnection::hgetall(const string key, map<string, string>& result)
{
	if(key.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_, "HGETALL %s", key.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}

	if ( (reply->type == REDIS_REPLY_ARRAY) && (reply->elements % 2 == 0) )
	{
		for (size_t i = 0; i < reply->elements; i += 2)
		{
			string field(reply->element[i]->str, reply->element[i]->len);
			string value(reply->element[i+1]->str, reply->element[i+1]->len);
			result.insert(make_pair(field, value));
		}
	}

	freeReplyObject(reply);
	return true;
}
//将一个或多个值 value 插入到列表 key 的表头
int RedisConnection::lpush(const string key,const string value)
{
	if(key.length()==0 || value.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,"LPUSH %s %s", key.c_str(),value.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}
//将一个或多个值 value 插入到列表 key 的表尾
int RedisConnection::rpush(const string key,const string value)
{
	if(key.length()==0 || value.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,"RPUSH %s %s", key.c_str(),value.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}

//移除并返回列表 key 的尾元素。
string RedisConnection::rpop(const string key)
{
	if(key.length()==0)
		return "";
	if(redisContext_ == NULL)
		return "";
	string result;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,"RPOP %s", key.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return "";
	}
	result.append(reply->str, reply->len);
	freeReplyObject(reply);
	return result;
}
//返回列表 key 的长度。如果 key 不存在，则 key 被解释为一个空列表，返回 0 .
int RedisConnection::llen(const string key)
{
	if(key.length()==0)
		return 0;
	if(redisContext_ == NULL)
		return 0;
	string chstr = "LLEN " + key;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return 0;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}

//将一个或多个 member 元素及其 score 值加入到有序集 key 当中
int RedisConnection::zadd(const string key,const string score_member)
{
	if(key.length()==0 || score_member.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	string chstr = "ZADD " + key + " " + score_member;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}
//返回有序集 key 中，指定区间内的成员。其中成员的位置按 score 值递增(从小到大)来排序。
bool RedisConnection::zrange(const string key,const int start,const int stop,vector<string>& result,bool ifscore)
{
	if(key.length()==0)
		return false;
	if(redisContext_ == NULL)
		return false;
	string chstr = "ZRANGE " + key + " " + Int_to_String(start) + " " + Int_to_String(stop);
	if(ifscore)
		chstr += " WITHSCORES";
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return false;
	}

	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (size_t i = 0; i < reply->elements; i ++)
		{
			string value(reply->element[i]->str,reply->element[i]->len);
			result.push_back(value);
		}
	}
	freeReplyObject(reply);
	return true;
}
//返回有序集 key 中成员 member 的排名。其中有序集成员按 score 值递增(从小到大)顺序排列。
//排名以 0 为底，也就是说， score 值最小的成员排名为 0 。
int RedisConnection::zrank(const string key,const string member)
{
	if(key.length()==0 ||member.length()==0)
		return -1;
	if(redisContext_ == NULL)
		return -1;
	string chstr = "ZRANK " + key + " " + member;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return -1;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}
//返回有序集 key 中成员 member 的排名。其中有序集成员按 score 值递减(从大到小)排序。
int RedisConnection::zrevrank(const string key,const string member)
{
	if(key.length()==0 ||member.length()==0)
		return -1;
	if(redisContext_ == NULL)
		return -1;
	string chstr = "ZREVRANK " + key + " " + member;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return -1;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}
//返回有序集 key 中，成员 member 的 score 值。
//如果 member 元素不是有序集 key 的成员，或 key 不存在，返回 nil 。
string RedisConnection::zscore(const string key,const string member)
{
	if(key.length()==0 || member.length()==0)
		return "";
	if(redisContext_ == NULL)
		return "";
	string chstr = "ZSCORE " + key + " " + member;
	string result = "";
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return "";
	}
	result.append(reply->str, reply->len);
	freeReplyObject(reply);
	return result;
}

//返回有序集 key 的基数。当 key 不存在时，返回 0 。
int RedisConnection::zcard(const string key)
{
	if(key.length()==0)
		return 0;
	if(redisContext_ == NULL)
		return 0;
	string chstr = "ZCARD " + key;
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return 0;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}
//计算给定的一个或多个有序集的交集
int RedisConnection::zinterstore(const string destkey,list<string>& srckeys)
{
	if(destkey.length()==0 || srckeys.size()==0)
		return 0;
	if(redisContext_ == NULL)
		return 0;
	int size = srckeys.size();
	string chstr = "ZINTERSTORE " + destkey + " " + Int_to_String(size);
	for(list<string>::iterator itkey = srckeys.begin();itkey != srckeys.end();++itkey)
	{
		chstr += " " + (*itkey);
	}
	redisReply* reply = static_cast<redisReply*>(redisCommand(redisContext_,chstr.c_str()));
	if (!checkReply(reply))
	{
		if (reply)
			freeReplyObject(reply);
		return 0;
	}
	int result = reply->integer;
	freeReplyObject(reply);
	return result;
}

////////////////////////RedisPool///////////////////////////////////////////////////////////

RedisPool::RedisPool(const string ip,
					uint16_t port,
					int minConn,
					int maxConn,
					int dbNo,
					const string nameArg,bool pifneedauth,const string pauthpwd)
{
	hostip_ = ip;
	hostport_ = port;
	minConn_ = minConn;
	maxConn_ = maxConn;
	dbNo_ = dbNo;
	name_ = nameArg;
	ifneedauth = pifneedauth;
	authpwd = pauthpwd;
	quit_ = false;
	pthread_mutex_init(&m_lock_cons,0);
}
RedisPool::~RedisPool()
{
	quit_ = true;
	for (list<RedisConnection*>::iterator it = connections_.begin(); it != connections_.end(); it++)
	{
		delete *it;
	}
	connections_.clear();
	minConn_= 0;
	pthread_mutex_destroy(&m_lock_cons);
}

int RedisPool::init()
{
	for (int i = 0; i < minConn_; i++)
	{
		RedisConnection* conn = new RedisConnection(this);
		conn->connect();
		if (conn != NULL)
			connections_.push_back(conn);
	}

	return 0;
}

// move out the disabled connections
void RedisPool::serverCron()
{
	while (!quit_)
	{

		list<RedisConnection*>::iterator it = connections_.begin();
		for (; it != connections_.end(); )
		{
			if ((*it)->ping() == false)
			{
				delete *it;
				connections_.remove(*it++);
			}
			else
			{
				it++;
			}
		}
	}
}
//从池中获取redis 连接
RedisConnection* RedisPool::getConnection()
{
	pthread_mutex_lock(&m_lock_cons);
	while (connections_.empty())
	{
		if (minConn_ >= maxConn_)
		{
			//暂时处理
			//可以加pthread_cond_wait pthread_cond_signal 处理
			RedisConnection* conn = new RedisConnection(this);
			conn->connect();
			connections_.push_back(conn);
			minConn_++;
		}
		else
		{
			RedisConnection* conn = new RedisConnection(this);
			conn->connect();
			connections_.push_back(conn);
			minConn_++;
		}
	}
	unsigned int nowtime = time(NULL);
	RedisConnection* pConn = connections_.front();
	connections_.pop_front();
	//判断连接是否有效
	if((nowtime-pConn->m_nowtime)>60){
		if(pConn->ping()==false){
			pConn->connect();
		}
		pConn->m_nowtime = nowtime;
	}
	pthread_mutex_unlock(&m_lock_cons);
	return pConn;
}
//把reids 连接放回池中
void RedisPool::freeConnection(RedisConnection* conn)
{
	pthread_mutex_lock(&m_lock_cons);
	list<RedisConnection*>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++)
	{
		if (*it == conn)
			break;
	}

	if (it == connections_.end())
	{
		connections_.push_back(conn);
	}
	pthread_mutex_unlock(&m_lock_cons);
}


