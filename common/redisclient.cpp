#include "redisclient.h"
#include <execinfo.h>
#include <stdlib.h>
#include <sstream>

namespace
{
	string Int_to_String(int n)
	{
		ostringstream stream;
		stream<<n;  //nΪint����
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
//���ù���ʱ��
void RedisClient::setExpire(const string key,unsigned int seconds)
{
	if(key.length()==0)
		return;
	RedisConnection* conn = redisPool_->getConnection();
	conn->setExpire(key, seconds);
	redisPool_->freeConnection(conn);
}
//�������з��ϸ���ģʽ pattern �� key
bool RedisClient::keys(const string pattern,list<string>& result)
{
	if(pattern.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool res = conn->keys(pattern,result);
	redisPool_->freeConnection(conn);
	return res;
}
//keyֵ�Ƿ����
bool RedisClient::exists(const string key)
{
	if(key.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool result = conn->exists(key);
	redisPool_->freeConnection(conn);
	return result;
}
//����key value
void RedisClient::set(const string key, const string value)
{
	if(key.length()==0)
		return;
	RedisConnection* conn = redisPool_->getConnection();
	conn->set(key, value);
	redisPool_->freeConnection(conn);
}
//Ϊ��ϣ�� key �е��� field ��ֵ�������� value
//����Ҳ����Ϊ�������൱�ڶԸ�������м�������
int RedisClient::hincrby(const string key, const string field, const string value)
{
	if(key.length()==0 || field.length()==0 || value.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->hincrby(key, field, value);
	redisPool_->freeConnection(conn);
	return result;
}
//��ȡvalue
string RedisClient::get(const string key)
{
	if(key.length()==0)
		return "";
	RedisConnection* conn = redisPool_->getConnection();
	string value = conn->get(key);
	redisPool_->freeConnection(conn);
	return value;
}
//hash���Ƿ���ڸ���
bool RedisClient::hexists(const string key, const string field)
{
	if(key.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool result = conn->hexists(key, field);
	redisPool_->freeConnection(conn);
	return result;
}
//����hash key value ����ֵ
int RedisClient::hset(const string key, const string field, const string value)
{
	if(key.length()==0 || field.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->hset(key, field, value);
	redisPool_->freeConnection(conn);
	return result;
}
//ȡHash ����ֵ
string RedisClient::hget(const string key, const string field)
{
	if(key.length()==0 || field.length()==0)
		return "";
	RedisConnection* conn = redisPool_->getConnection();
	string result = conn->hget(key, field);
	redisPool_->freeConnection(conn);
	return result;
}
//����Hash ���ֵ
int RedisClient::hmset(const string key, const string field_value)
{
	if(key.length()==0 || field_value.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->hmset(key, field_value);
	redisPool_->freeConnection(conn);
	return result;
}
//ȡHash ���ֵ
bool RedisClient::hmget(const string key, const string field,list<string>& result)
{
	if(key.length()==0 || field.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->hmget(key, field,result);
	redisPool_->freeConnection(conn);
	return ret;
}
//���ع�ϣ�� key �У����е����ֵ
bool RedisClient::hgetall(const string key, map<string, string>& result)
{
	if(key.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->hgetall(key, result);
	redisPool_->freeConnection(conn);
	return ret;
}
//��һ������ֵ value ���뵽�б� key �ı�ͷ
int RedisClient::lpush(const string key,const string value)
{
	if(key.length()==0 || value.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->lpush(key, value);
	redisPool_->freeConnection(conn);
	return ret;
}
//��һ������ֵ value ���뵽�б� key �ı�β
int RedisClient::rpush(const string key,const string value)
{
	if(key.length()==0 || value.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->rpush(key, value);
	redisPool_->freeConnection(conn);
	return ret;
}

//�Ƴ��������б� key ��βԪ�ء�
string RedisClient::rpop(const string key)
{
	if(key.length()==0)
		return "";
	RedisConnection* conn = redisPool_->getConnection();
	string result = conn->rpop(key);
	redisPool_->freeConnection(conn);
	return result;
}
//�����б� key �ĳ��ȡ���� key �����ڣ��� key ������Ϊһ�����б����� 0 .
int RedisClient::llen(const string key)
{
	if(key.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->llen(key);
	redisPool_->freeConnection(conn);
	return result;
}
//��һ������ member Ԫ�ؼ��� score ֵ���뵽���� key ����
int RedisClient::zadd(const string key,const string score_member)
{
	if(key.length()==0 || score_member.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zadd(key, score_member);
	redisPool_->freeConnection(conn);
	return result;
}
//�������� key �У�ָ�������ڵĳ�Ա�����г�Ա��λ�ð� score ֵ����(��С����)������
bool RedisClient::zrange(const string key,const int start,const int stop,vector<string>& result,bool ifscore)
{
	if(key.length()==0)
		return false;
	RedisConnection* conn = redisPool_->getConnection();
	bool ret = conn->zrange(key, start,stop,result,ifscore);
	redisPool_->freeConnection(conn);
	return ret;
}
//�������� key �г�Ա member ���������������򼯳�Ա�� score ֵ����(��С����)˳�����С�
//������ 0 Ϊ�ף�Ҳ����˵�� score ֵ��С�ĳ�Ա����Ϊ 0 ��
int RedisClient::zrank(const string key,const string member)
{
	if(key.length()==0 || member.length()==0)
		return -1;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zrank(key, member);
	redisPool_->freeConnection(conn);
	return result;
}
//�������� key �г�Ա member ���������������򼯳�Ա�� score ֵ�ݼ�(�Ӵ�С)����
//������ 0 Ϊ�ף�Ҳ����˵�� score ֵ��С�ĳ�Ա����Ϊ 0 ��
int RedisClient::zrevrank(const string key,const string member)
{
	if(key.length()==0 || member.length()==0)
		return -1;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zrevrank(key, member);
	redisPool_->freeConnection(conn);
	return result;
}
//�������� key �У���Ա member �� score ֵ��
//��� member Ԫ�ز������� key �ĳ�Ա���� key �����ڣ����� nil ��
string RedisClient::zscore(const string key,const string member)
{
	if(key.length()==0 || member.length()==0)
		return "";
	RedisConnection* conn = redisPool_->getConnection();
	string result = conn->zscore(key, member);
	redisPool_->freeConnection(conn);
	return result;
}

//�������� key �Ļ������� key ������ʱ������ 0 ��
int RedisClient::zcard(const string key)
{
	if(key.length()==0)
		return 0;
	RedisConnection* conn = redisPool_->getConnection();
	int result = conn->zcard(key);
	redisPool_->freeConnection(conn);
	return result;
}
//���������һ���������򼯵Ľ���
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
//����redis
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
//��鷵��ֵ
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
//���ù���ʱ��
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

//ping �Ƿ����ӻ���Ч
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
//�������з��ϸ���ģʽ pattern �� key
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
//�Ƿ����key
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
//����key value
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
//��ȡvalue
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
//Ϊ��ϣ�� key �е��� field ��ֵ�������� value
//����Ҳ����Ϊ�������൱�ڶԸ�������м�������
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
//hash���Ƿ���ڸ���
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
//����hash һ��ֵ
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
//ȡHash ����ֵ
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
//����Hash ���ֵ
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
//ȡHash ���ֵ
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
//���ع�ϣ�� key �У����е����ֵ��
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
//��һ������ֵ value ���뵽�б� key �ı�ͷ
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
//��һ������ֵ value ���뵽�б� key �ı�β
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

//�Ƴ��������б� key ��βԪ�ء�
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
//�����б� key �ĳ��ȡ���� key �����ڣ��� key ������Ϊһ�����б����� 0 .
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

//��һ������ member Ԫ�ؼ��� score ֵ���뵽���� key ����
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
//�������� key �У�ָ�������ڵĳ�Ա�����г�Ա��λ�ð� score ֵ����(��С����)������
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
//�������� key �г�Ա member ���������������򼯳�Ա�� score ֵ����(��С����)˳�����С�
//������ 0 Ϊ�ף�Ҳ����˵�� score ֵ��С�ĳ�Ա����Ϊ 0 ��
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
//�������� key �г�Ա member ���������������򼯳�Ա�� score ֵ�ݼ�(�Ӵ�С)����
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
//�������� key �У���Ա member �� score ֵ��
//��� member Ԫ�ز������� key �ĳ�Ա���� key �����ڣ����� nil ��
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

//�������� key �Ļ������� key ������ʱ������ 0 ��
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
//���������һ���������򼯵Ľ���
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
//�ӳ��л�ȡredis ����
RedisConnection* RedisPool::getConnection()
{
	pthread_mutex_lock(&m_lock_cons);
	while (connections_.empty())
	{
		if (minConn_ >= maxConn_)
		{
			//��ʱ����
			//���Լ�pthread_cond_wait pthread_cond_signal ����
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
	//�ж������Ƿ���Ч
	if((nowtime-pConn->m_nowtime)>60){
		if(pConn->ping()==false){
			pConn->connect();
		}
		pConn->m_nowtime = nowtime;
	}
	pthread_mutex_unlock(&m_lock_cons);
	return pConn;
}
//��reids ���ӷŻس���
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


