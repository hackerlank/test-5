#ifndef OPTIONS_H
#define OPTIONS_H

#include "../common/Ini_File.h"
#include "../common/Pubdata.h"

typedef struct _gw_ip
{
	char 	ip_gw[16];
	int		port_gw;
}gw_ip;

struct Ini_Option_t
{
	char	log_dir[128];
	char	log_prename[32];
	int 	log_size;
	int		log_num;
	short	log_priority;

	int		listen_count;		//如果为0，则本身不提供外部连接处理
	char	listen_ip[16];
	int		listen_port;
	int		socket_timeout;
	int		socket_bufsize;
	int		backlog;

	int		server_type;
	char 	ip_lobby[16];
	int		port_lobby;

	int		gw_count;
	gw_ip	gw[MAX_GW_COUNT];
	int		socket_bufsize_gw;

	char	mysql_ip[16];
	char	mysql_user[16];
	char	mysql_passwd[16];
	char	mysql_db[16];
	int		mysql_port;

	//redis ip port pwd
	char redis_ip[16];
	short redis_port;
	char redis_pwd[64];
	char redis_ifauth;//是否需要密码验证1 需要
};

class Option
{
public:
	Option();
	~Option();

	int init(const char* config_file);
	
	int bind_count;
protected:
	int	parse_ini();

	IniFile ini_file;
};

extern Ini_Option_t ini;
extern Option option;
#endif

