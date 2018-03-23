#include "Options.h"
#include <string>
#include <stdio.h>
using namespace std;
	
Ini_Option_t ini;
Option::Option()
{
	memset (&ini, 0x0, sizeof (ini));
	bind_count = 0;
}

Option::~Option()
{
}

int Option::init(const char* config_file)
{
	if (!ini_file.open (config_file))
	{
		printf ("open config file:%s failed\n", config_file);
		return -1;
	}

	if (parse_ini () != 0)
		return -1;
	
	return 0;
}

int	Option::parse_ini()
{
	string tmp;

	tmp = ini_file.read ("path", "LOG_DIR");
	strncpy (ini.log_dir, tmp.c_str(), sizeof ini.log_dir);

	tmp = ini_file.read ("logger", "LOG_MAX_SIZE");
	if (tmp.empty ())
		return -1;
	ini.log_size = atoi (tmp.c_str());

	tmp = ini_file.read ("logger", "LOG_PRENAME");
	if (tmp.empty ())
		memset (ini.log_prename, 0x0, sizeof ini.log_prename);
	else
		strncpy (ini.log_prename, tmp.c_str(), sizeof ini.log_prename);

	tmp = ini_file.read ("logger", "LOG_MAX_NUM");
	if (tmp.empty ())
		return -1;
	ini.log_num = atoi (tmp.c_str());

	tmp = ini_file.read ("logger", "LOG_PRIORITY");
	if (tmp.empty ())
		return -1;
	ini.log_priority = atoi (tmp.c_str());

	tmp = ini_file.read ("misc", "SOCKET_TIMEOUT");
	if (tmp.empty ())
		return -1;
	ini.socket_timeout = atoi (tmp.c_str());

	tmp = ini_file.read ("misc", "SOCKET_BUFSIZE");
	if (tmp.empty ())
		return -1;
	ini.socket_bufsize = atoi (tmp.c_str());

	tmp = ini_file.read ("misc", "ACCEPT_BACKLOG");
	if (tmp.empty ())
		ini.backlog = 10;
	else
		ini.backlog = atoi (tmp.c_str());
	
	tmp = ini_file.read ("misc", "LISTEN_IP");
	strncpy (ini.listen_ip, tmp.c_str(), sizeof ini.listen_ip);

	tmp = ini_file.read ("misc", "LISTEN_PORT");
	if (tmp.empty ())
		return -1;
	else
		ini.listen_port = atoi (tmp.c_str());

	tmp = ini_file.read ("misc", "LISTEN_COUNT");
	if (tmp.empty ())
		return -1;
	else
		ini.listen_count = atoi (tmp.c_str());

	tmp = ini_file.read ("gw", "SOCKET_BUFSIZE_GW");
	if (tmp.empty ())
		return -1;
	else
		ini.socket_bufsize_gw = atoi (tmp.c_str());

	tmp = ini_file.read ("gw", "COUNT");
	if (tmp.empty ())
		return -1;
	else
		ini.gw_count = atoi (tmp.c_str());
	if(ini.gw_count > MAX_GW_COUNT)
		return -1;

	for(int i=0; i<ini.gw_count; i++)
	{
		char s_name_tmp[16+1]={0};
		snprintf(s_name_tmp, 16, "IP_%d", i);
		tmp = ini_file.read ("gw", s_name_tmp);
		if (tmp.empty ())
			return -1;
		strncpy (ini.gw[i].ip_gw, tmp.c_str(), 16);

		memset(s_name_tmp, 0, sizeof(s_name_tmp));
		snprintf(s_name_tmp, 16, "PORT_%d", i);
		tmp = ini_file.read ("gw", s_name_tmp);
		if (tmp.empty ())
			return -1;
		else
			ini.gw[i].port_gw = atoi (tmp.c_str());
	}

	tmp = ini_file.read ("MYSQL", "IP");
	if (tmp.empty ())
		return -1;
	else
		strncpy (ini.mysql_ip, tmp.c_str(), sizeof ini.mysql_ip);

	tmp = ini_file.read ("MYSQL", "PORT");
	if (tmp.empty ())
		return -1;
	else
		ini.mysql_port = atoi (tmp.c_str());

	tmp = ini_file.read ("MYSQL", "USERNAME");
	if (tmp.empty ())
		return -1;
	else
		strncpy (ini.mysql_user, tmp.c_str(), sizeof ini.mysql_user);

	tmp = ini_file.read ("MYSQL", "PASSWORD");
	if (tmp.empty ())
		return -1;
	else
		strncpy (ini.mysql_passwd, tmp.c_str(), sizeof ini.mysql_passwd);

	tmp = ini_file.read ("MYSQL", "DB");
	if (tmp.empty ())
		return -1;
	else
		strncpy (ini.mysql_db, tmp.c_str(), sizeof ini.mysql_db);

	tmp = ini_file.read ("net", "IP_LOBBY");
	if (tmp.empty ())
		return -1;
	strncpy (ini.ip_lobby, tmp.c_str(), sizeof ini.ip_lobby);

	tmp = ini_file.read ("net", "PORT_LOBBY");
	if (tmp.empty ())
		return -1;
	else
		ini.port_lobby = atoi (tmp.c_str());

	tmp = ini_file.read ("net", "SERVER_TYPE");
	if (tmp.empty ())
		return -1;
	else
		ini.server_type = atoi (tmp.c_str());
	// redis
	tmp = ini_file.read ("redis", "REDIS_IP");
	if (tmp.empty ())
		return -1;
	else
		strncpy (ini.redis_ip, tmp.c_str(), sizeof ini.redis_ip);

	tmp = ini_file.read ("redis", "REDIS_PORT");
	if (tmp.empty ())
		return -1;
	else
		ini.redis_port= atoi (tmp.c_str());
	
	tmp = ini_file.read ("redis", "REDIS_PWD");
	if (tmp.empty ())
		return -1;
	else
		strncpy (ini.redis_pwd, tmp.c_str(), sizeof ini.redis_pwd);
	
	tmp = ini_file.read ("redis", "REDIS_IFAUTH");
	if (tmp.empty ())
		return -1;
	else
		ini.redis_ifauth = atoi (tmp.c_str());
	return 0;
}

