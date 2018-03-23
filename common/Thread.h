#ifndef _THREAD_H_
#define _THREAD_H_


#include <pthread.h>
#include <mysql.h>
#include "Ini_File.h"
#include "../common/redisclient.h"


#define QUENE_SIZE			2048

typedef struct _queue_msg
{
	int fd;			//fd
	char IsInner;	// 1:内网2:外网
	int length;		//本次请求长度
	char* recv_mb;	//接收数据缓存
	//mysql句柄
	MYSQL mysql;//
	//当前时间
	unsigned int nowtime ;
	//RedisConnection 指针
	RedisConnection* rediscon;
	//RedisPool 指针
	RedisPool* redispool;
	//数据库连接
	int connect()
	{
		mysql_init(&mysql);
		if(NULL == mysql_real_connect(&mysql,ini.mysql_ip,ini.mysql_user, ini.mysql_passwd, ini.mysql_db,ini.mysql_port,NULL,CLIENT_MULTI_STATEMENTS))
		{
			return -1;
		}
		char sql_string[128+1] = {0};
		snprintf(sql_string, sizeof(sql_string) - 1, "set names utf8");
		if (mysql_real_query(&mysql, sql_string, strlen(sql_string)) != 0)
			return -1;
		//my_bool my_true = true;
		//mysql_options(&mysql, MYSQL_OPT_RECONNECT, &my_true);
		return 0;
	}
	
	_queue_msg()
	{
		recv_mb = (char *) malloc (LEN_SINGLE_MSG);
		nowtime = 0;
		rediscon = NULL;
		redispool = NULL;
	}
	~_queue_msg()
	{
		if (recv_mb != NULL) {
			free (recv_mb);
			recv_mb = NULL;
		}
		if (rediscon != NULL) {
			delete rediscon;
			rediscon = NULL;
		}
		if (redispool != NULL) {
			delete redispool;
			redispool = NULL;
		}
	}
	//执行一些操作
	bool execSql(const char* chSql)
	{
		if(mysql_real_query(&mysql,chSql,strlen(chSql))==0)
        {
               	return   true;
		}
		return false;
	}
	//取出结果集
	MYSQL_RES* openRecordset(const char* chSql)
	{
		MYSQL_RES   *rs   =   NULL;
		if(execSql(chSql))
		{
		      rs = mysql_store_result(&mysql);
		}
		return rs;
	}
	//释放结果集
	void freeResult(MYSQL_RES* result)
	{
		if(result)
                mysql_free_result(result);
	}
	//连接状态0 保持连接
	int connectStatus()
	{
		int pingres = mysql_ping(&mysql);
		return pingres;
	}
	//mysql关闭连接
	void closeMysql()
	{
		 mysql_close(&mysql);
	}
	//redis 连接初始化
	void redisinit(string pname = "common")
	{
		if(ini.redis_ifauth)
			redispool = new RedisPool(ini.redis_ip, ini.redis_port, 1, 1, 0, pname,true,ini.redis_pwd);
		else
			redispool = new RedisPool(ini.redis_ip, ini.redis_port, 1, 1, 0, pname);
		rediscon = new RedisConnection(redispool);
		rediscon->connect();
	}
	//redis 连接
	int redisconnect()
	{
		if(rediscon == NULL || redispool == NULL)
			return -1;
		unsigned int tmp_time = time(NULL);
		//判断连接是否有效
		if((tmp_time-rediscon->m_nowtime)>60){
			if(rediscon->ping()==false){
				rediscon->connect();
			}
			rediscon->m_nowtime = tmp_time;
		}
		return 0;
	}
}queue_msg;


class CThreadQueue
{
public:
    CThreadQueue(int queueSize=QUENE_SIZE):
        sizeQueue(queueSize),lput(0),lget(0),nFullThread(0),nEmptyThread(0),nData(0)
    {
        pthread_mutex_init(&mux,0);
        pthread_cond_init(&condGet,0);
        pthread_cond_init(&condPut,0);
		queue_begin = new queue_msg[sizeQueue];
    }
    virtual ~CThreadQueue()
    {
        delete []queue_begin;
		pthread_mutex_destroy(&mux);
		pthread_cond_destroy(&condGet);
		pthread_cond_destroy(&condPut);
    }
	void getq(queue_msg* data)
    {
        pthread_mutex_lock(&mux);
		//此处循环判断的原因如下：假设2个线程在getq阻塞，
		//然后两者都被激活，而其中一个线程运行比较块，
		//快速消耗了2个数据，另一个线程醒来的时候已经没有
		//新数据可以消耗了。另一点，man pthread_cond_wait可以看到，
		//该函数可以被信号中断返回，此时返回EINTR。
		//为避免以上任何一点，都必须醒来后再次判断睡眠条件。
		//更正：pthread_cond_wait是信号安全的系统调用，不会被信号中断。
        while(lget==lput&&nData==0)
        {
            nEmptyThread++;
            pthread_cond_wait(&condGet,&mux);
            nEmptyThread--;     
        }
		data->fd = (queue_begin + lget)->fd;
		data->IsInner = (queue_begin + lget)->IsInner;
		data->length = (queue_begin + lget)->length;
		memcpy(data->recv_mb, (queue_begin + lget)->recv_mb, data->length);
		lget++;
        nData--;
        if(lget==sizeQueue)
        {
            lget=0;
        }
        if(nFullThread) //必要时才进行signal操作，勿总是signal
        {
            pthread_cond_signal(&condPut);    
        }
        pthread_mutex_unlock(&mux);
    }
    void putq(queue_msg *data)
    {
        pthread_mutex_lock(&mux);
        while(lput==lget&&nData)
        { 
            nFullThread++;
            pthread_cond_wait(&condPut,&mux);
            nFullThread--;
        }
		(queue_begin+lput)->fd = data->fd;
		(queue_begin+lput)->IsInner = data->IsInner;
		(queue_begin+lput)->length = data->length;
		memcpy((queue_begin + lput)->recv_mb,data->recv_mb, data->length);
		lput++;
        nData++;
        if(lput==sizeQueue)
        {
            lput=0;
        }
        if(nEmptyThread)
        {
            pthread_cond_signal(&condGet);
        }
        pthread_mutex_unlock(&mux);
    }
private:
    pthread_mutex_t mux;
    pthread_cond_t condGet;
    pthread_cond_t condPut;

    queue_msg* queue_begin;//循环消息队列
    int sizeQueue;        //队列大小
    int lput;        //location put  放数据的指针偏移
    int lget;        //location get  取数据的指针偏移
    int nFullThread;    //队列满，阻塞在putq处的线程数
    int nEmptyThread;    //队列空，阻塞在getq处的线程数
    int nData;        //队列中的消息个数，主要用来判断队列空还是满
};

#endif

