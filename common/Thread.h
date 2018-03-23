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
	char IsInner;	// 1:����2:����
	int length;		//�������󳤶�
	char* recv_mb;	//�������ݻ���
	//mysql���
	MYSQL mysql;//
	//��ǰʱ��
	unsigned int nowtime ;
	//RedisConnection ָ��
	RedisConnection* rediscon;
	//RedisPool ָ��
	RedisPool* redispool;
	//���ݿ�����
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
	//ִ��һЩ����
	bool execSql(const char* chSql)
	{
		if(mysql_real_query(&mysql,chSql,strlen(chSql))==0)
        {
               	return   true;
		}
		return false;
	}
	//ȡ�������
	MYSQL_RES* openRecordset(const char* chSql)
	{
		MYSQL_RES   *rs   =   NULL;
		if(execSql(chSql))
		{
		      rs = mysql_store_result(&mysql);
		}
		return rs;
	}
	//�ͷŽ����
	void freeResult(MYSQL_RES* result)
	{
		if(result)
                mysql_free_result(result);
	}
	//����״̬0 ��������
	int connectStatus()
	{
		int pingres = mysql_ping(&mysql);
		return pingres;
	}
	//mysql�ر�����
	void closeMysql()
	{
		 mysql_close(&mysql);
	}
	//redis ���ӳ�ʼ��
	void redisinit(string pname = "common")
	{
		if(ini.redis_ifauth)
			redispool = new RedisPool(ini.redis_ip, ini.redis_port, 1, 1, 0, pname,true,ini.redis_pwd);
		else
			redispool = new RedisPool(ini.redis_ip, ini.redis_port, 1, 1, 0, pname);
		rediscon = new RedisConnection(redispool);
		rediscon->connect();
	}
	//redis ����
	int redisconnect()
	{
		if(rediscon == NULL || redispool == NULL)
			return -1;
		unsigned int tmp_time = time(NULL);
		//�ж������Ƿ���Ч
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
		//�˴�ѭ���жϵ�ԭ�����£�����2���߳���getq������
		//Ȼ�����߶������������һ���߳����бȽϿ飬
		//����������2�����ݣ���һ���߳�������ʱ���Ѿ�û��
		//�����ݿ��������ˡ���һ�㣬man pthread_cond_wait���Կ�����
		//�ú������Ա��ź��жϷ��أ���ʱ����EINTR��
		//Ϊ���������κ�һ�㣬�������������ٴ��ж�˯��������
		//������pthread_cond_wait���źŰ�ȫ��ϵͳ���ã����ᱻ�ź��жϡ�
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
        if(nFullThread) //��Ҫʱ�Ž���signal������������signal
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

    queue_msg* queue_begin;//ѭ����Ϣ����
    int sizeQueue;        //���д�С
    int lput;        //location put  �����ݵ�ָ��ƫ��
    int lget;        //location get  ȡ���ݵ�ָ��ƫ��
    int nFullThread;    //��������������putq�����߳���
    int nEmptyThread;    //���пգ�������getq�����߳���
    int nData;        //�����е���Ϣ��������Ҫ�����ж϶��пջ�����
};

#endif

