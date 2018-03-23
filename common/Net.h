#ifndef NET_H
#define NET_H
#include "Common.h"

int socket_send(int sockfd, const char *buffer, int buflen);

int send_tcp_session (Connect_Session_t* s);

int recv_tcp_buffer (Connect_Session_t* s);

int open_tcp_port (const char* ip, short port, int backlog);

int accept_tcp (int listenfd, struct sockaddr_in *peer);



#endif



