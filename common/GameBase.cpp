#include "GameBase.h"

//消费线程，读取网络信息队列
void * GameLogicBase::queue_consume(void * object)
{
	pthread_detach(pthread_self());
	GameLogicBase *p_logic_processor = (GameLogicBase *)object;
	p_logic_processor->init();
	queue_msg data;
	data.redisinit("queue_consume");
	data.connect();
	while(!stopped)
	{
		p_logic_processor->m_thread_queue.getq(&data);
		p_logic_processor->process_one_msg(&data);
	}
	return 0;
}
//时间线程
void * GameLogicBase::time_pthread(void * object)
{
	pthread_detach(pthread_self());
	GameLogicBase *p_logic_processor = (GameLogicBase *)object;
	queue_msg data;
	data.fd = 0;
	data.IsInner = 1;
	data.length = LEN_PKG_HEAD;
	pkg_head pkg_head_input;
	pkg_head_input.command = 1;
	pkg_head_input.packet(data.recv_mb);
	data.connect();
	data.redisinit("time_pthread");
	struct timeval tempval;
	while(!stopped) {
		tempval.tv_sec = 1;
		tempval.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tempval);
		p_logic_processor->process_time_thread(&data);
	}
	if(mysql_ping(&data.mysql) != 0)
		mysql_close(&data.mysql);

	return 0;
}
//数据落地请求大厅
int GameLogicBase::req_settle(int user_id, char add_reduce, int coin,char logtype,char coin_type)
{
	int nReturn = 0;

	if(m_scoket_lobby < 0)
	{
		if(connect_lobby() != 0)
		{
			return -1;
		}
	}

	pkg_head pkg_head_settle;
	pkg_head_settle.type = TYPE_LOBBY;
	pkg_head_settle.command = COMMAND_LOBBY_GAME_SETTLE;
	pkg_head_settle.length = LEN_PKG_HEAD;
	pkg_head_settle.uid = user_id;
	char buffer_settle[LEN_BUFF_DATA_64]={0};
	memcpy(buffer_settle+pkg_head_settle.length, &add_reduce, 1);
	pkg_head_settle.length += 1;
	memcpy(buffer_settle+pkg_head_settle.length, &coin, 4);
	pkg_head_settle.length += 4;
	memcpy(buffer_settle+pkg_head_settle.length, &logtype, 1);
	pkg_head_settle.length += 1;
	memcpy(buffer_settle+pkg_head_settle.length, &ini.server_type, 4);
	pkg_head_settle.length += 4;
	memcpy(buffer_settle+pkg_head_settle.length, &coin_type, 1);
	pkg_head_settle.length += 1;
	pkg_head_settle.packet(buffer_settle);
	if(pkg_head_settle.length != socket_send(m_scoket_lobby, buffer_settle, pkg_head_settle.length))		
	{
		LOG (LOG_ERROR,  __FILE__, __LINE__, "send room[%s:%d] error[%s]", ini.ip_lobby,ini.port_lobby, strerror(errno));
		close(m_scoket_lobby);
		m_scoket_lobby = -1;
		if(connect_lobby() != 0)
		{
			nReturn  = ERROR_COMMON_SERVER;
		}else{
			if(pkg_head_settle.length != socket_send(m_scoket_lobby, buffer_settle, pkg_head_settle.length))
			{
				close(m_scoket_lobby);
				m_scoket_lobby = -1;
				nReturn  = ERROR_COMMON_SERVER;
				LOG (LOG_ERROR,  __FILE__, __LINE__, "send room[%s:%d] error[%s]", ini.ip_lobby,ini.port_lobby, strerror(errno));
			}
		}
	}
	if(0==nReturn && m_coin_type==coin_type){
		map<int, UserGameInfoBase>::iterator pos_user = m_map_user.find(user_id);
		if(pos_user != m_map_user.end()){
			pos_user->second.opcoin(coin,add_reduce);
		}
	}
	return nReturn;
}
//踢掉用户
int GameLogicBase::req_kick_user(int user_id)
{
	if(m_scoket_lobby < 0)
	{
		if(connect_lobby() != 0)
		{
			return -1;
		}
	}
	
	int nCount = m_map_user.size();
	int nReturn = 0;

	pkg_head pkg_head_settle;
	pkg_head_settle.type = TYPE_LOBBY;
	pkg_head_settle.command = COMMAND_LOBBY_GAME_USER_LEFT;
	pkg_head_settle.length = LEN_PKG_HEAD+4+4;
	pkg_head_settle.uid = user_id;
	char buffer_settle[LEN_PKG_HEAD+4+4]={0};
	pkg_head_settle.packet(buffer_settle);
	memcpy(buffer_settle+LEN_PKG_HEAD, &ini.server_type, 4);
	memcpy(buffer_settle+LEN_PKG_HEAD+4, &nCount, 4);
	if ( pkg_head_settle.length != socket_send(m_scoket_lobby, buffer_settle, pkg_head_settle.length))		
	{
		LOG (LOG_ERROR,  __FILE__, __LINE__, "send room[%s:%d] error[%s]", ini.ip_lobby,ini.port_lobby, strerror(errno));
		close(m_scoket_lobby);
		m_scoket_lobby = -1;
		if(connect_lobby() != 0)
		{
			nReturn  = ERROR_COMMON_SERVER;
		}else{
			if(pkg_head_settle.length != socket_send(m_scoket_lobby, buffer_settle, pkg_head_settle.length))
			{
				close(m_scoket_lobby);
				m_scoket_lobby = -1;
				nReturn  = ERROR_COMMON_SERVER;
			}
		}
	}

	return nReturn;
}
//连接大厅
int GameLogicBase::connect_lobby()
{
	struct	sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;	
	servaddr.sin_port = htons(ini.port_lobby);
	inet_pton(AF_INET, ini.ip_lobby, &servaddr.sin_addr);
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connect (sockfd, (const struct sockaddr *)&servaddr, sizeof (servaddr)) == 0)	
	{
		struct timeval timeout = {1, 0};
		setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)& timeout, sizeof (struct timeval));
		m_scoket_lobby = sockfd;
		LOG (LOG_DEBUG,  __FILE__, __LINE__, "connect lobby[%s:%d] success", 
			ini.ip_lobby,ini.port_lobby);
		usleep(10000);
		return 0;
	}
	else
	{
		m_scoket_lobby = -1;
		LOG (LOG_ERROR,  __FILE__, __LINE__, "connect lobby[%s:%d] error[%s]", 
			ini.ip_lobby,ini.port_lobby, strerror(errno));
		return -1;
	}

	return -1;
}
//加载数据库中初始化数据
void GameLogicBase::loadDbInitData()
{
	string strsql = "";
	int i = 0;
	char sql_string[LEN_BUFF_DATA_1k] = {0};
	snprintf(sql_string, sizeof(sql_string)-1, "select base_limit, top_limit, commission,enter_limit,basefee,coin_type,base_times,game_type,basepoint,room_level,robotwin,ulosecoin,uwincoin from mm_room where room_id = %d;", ini.server_type);
	strsql += sql_string;
	MYSQL_RES 	*res;
	MYSQL_ROW row = NULL;
	if(mysql_real_query(&m_mysql, strsql.c_str(), strlen(strsql.c_str())) == 0) 
	{
		i = 0;
		do{
			res = mysql_store_result(&m_mysql);
			if(i==0){
				if((row = mysql_fetch_row(res)) != NULL)
				{
					m_min_coin = atoi(row[0]);
					m_coin_limit = atoi(row[1]);
					m_commission = atoi(row[2]);
					m_enter_limit = atoi(row[3]);
					m_basefee = atoi(row[4]);
					m_coin_type = atoi(row[5]);
					m_basetimes = atoi(row[6]);
					m_gametype = atoi(row[7]);
					m_baseintegral = atoi(row[8]);
					m_roomlevel = atoi(row[9]);
					m_robotwinrate = atoi(row[10]);
					m_usermaxlosecoin = atoi(row[11]);
					m_usermaxwincoin = atoi(row[12]);
					LOG (LOG_DEBUG, __FILE__, __LINE__, "room_id=%d, min_coin=%d, basefee=%d, usermaxlosecoin=%d,usermaxwincoin=%d,enter_limit=%d",
						ini.server_type, m_min_coin, m_basefee, m_usermaxlosecoin,m_usermaxwincoin,m_enter_limit);
				}
			}
			mysql_free_result(res);
			i++;
		}while(mysql_next_result(&m_mysql) == 0);
	}
}
//更新机器人胜率
void GameLogicBase::updateRobotWinRate(queue_msg *data)
{
	unsigned int lastrate = m_robotwinrate;
	int nReturn = 0;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row = NULL;
	string strsql = "";
	int i = 0;
	char sql_string[LEN_BUFF_DATA_256] = {0};
	snprintf(sql_string, sizeof(sql_string)-1, "select robotwin,ulosecoin,uwincoin from mm_room where room_id = %d limit 1;",
		ini.server_type);
	strsql += sql_string;
	bool sucess = data->execSql(strsql.c_str());
	if (sucess) {
		do{
			res = mysql_store_result(&data->mysql);
			if(i == 0)
			{
				if((row = mysql_fetch_row(res)) != NULL)  {
				   	m_robotwinrate = atoi(row[0]);
				   	m_usermaxlosecoin = atoi(row[1]);
					m_usermaxwincoin = atoi(row[2]);
				}
			}
			mysql_free_result(res);
		}while(mysql_next_result(&data->mysql) == 0);
	} else {
		LOG (LOG_ERROR, __FILE__, __LINE__, "database error: [%s]--[%s]", mysql_error(&data->mysql), sql_string);
		nReturn = ERROR_COMMON_SERVER;
	}
	LOG (LOG_DEBUG, __FILE__, __LINE__,"updateRobotWinRate,roomid=%d,robotwinrate=%d,lastrate=%d"
		,ini.server_type,m_robotwinrate,lastrate);
}
//更新机器人输赢金币概率
void GameLogicBase::updateRobotLoseWincoin()
{
	string field = "robotmaxlosecoin robotmaxwincoin robotwincoin";
	list<string> result;
	redisClient->hmget("global.data", field, result);
	list<string>::iterator valueIt = result.begin();
	if(valueIt != result.end()){
		m_robotmaxlosecoin = atoi((*valueIt).c_str());
		valueIt++;
	}
	if(valueIt != result.end()){
		m_robotmaxwincoin = atoi((*valueIt).c_str());
		valueIt++;
	}
	if(valueIt != result.end()){
		m_robotcurwincoin = atoi((*valueIt).c_str());
		valueIt++;
	}
	int baseprob = abs(m_robotmaxwincoin - m_robotmaxlosecoin);
	int curbase = m_robotcurwincoin - m_robotmaxlosecoin;
	if(curbase<=0){
		curbase = 0;
	}
	if(curbase>baseprob){
		curbase = baseprob;
	}
	if(baseprob<=0){
		m_robotcurwinrate = 0;
	}else{
		m_robotcurwinrate = (float)curbase/(float)baseprob * 10000;
	}
}
//获取用户信息
void GameLogicBase::get_user_info(string userkey,UserGameInfoBase& user_status_info)
{
	string field = "uid nickname coinnum integral ingotnum headurl gender robotwincoin";
	list<string> result;
	redisClient->hmget(userkey, field, result);
	list<string>::iterator valueIt = result.begin();
	string nickename = "",headurl="";
	unsigned int uid = 0,coinnum=0,integral=0,ingotnum=0,gender=0,robotwincoin;
	if(valueIt != result.end()){
		uid = atoi((*valueIt).c_str());
		valueIt++;
	}
	if(valueIt != result.end()){
		nickename = (*valueIt);
		valueIt++;
	}
	if(valueIt != result.end()){
		coinnum = atoi((*valueIt).c_str());
		valueIt++;
	}
	if(valueIt != result.end()){
		integral = atoi((*valueIt).c_str());
		valueIt++;
	}
	if(valueIt != result.end()){
		ingotnum = atoi((*valueIt).c_str());
		valueIt++;
	}
	if(valueIt != result.end()){
		headurl = (*valueIt);
		valueIt++;
	}
	if(valueIt != result.end()){
		gender = atoi((*valueIt).c_str());
		valueIt++;
	}
	if(valueIt != result.end()){
		robotwincoin = atoi((*valueIt).c_str());
		valueIt++;
	}
	user_status_info.nickname = nickename;
	user_status_info.headurl = headurl;
	user_status_info.gender = gender;
	user_status_info.integral = integral;
	user_status_info.robotwincoin = robotwincoin;
	if(m_coin_type == CoinType_Coin){
		user_status_info.coin = coinnum;
	}else if(m_coin_type == CoinType_Integral){
		user_status_info.coin = integral;
	}
}