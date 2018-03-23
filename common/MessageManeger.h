#ifndef __MESSAGEMANEGER_H__
/*
desc:通讯消息相关
writer:Lake
*/
#define __MESSAGEMANEGER_H__

#include "Thread.h"

class lobby_logic_processor;

//消息处理接口
class IMessageHandler{
	public:
		virtual bool process_one_msg(queue_msg *data) = 0;//处理消息
		
		virtual ~IMessageHandler(){}
	public:
		lobby_logic_processor *lobbyprocess;//大厅处理器
};
#endif

