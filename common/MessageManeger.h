#ifndef __MESSAGEMANEGER_H__
/*
desc:ͨѶ��Ϣ���
writer:Lake
*/
#define __MESSAGEMANEGER_H__

#include "Thread.h"

class lobby_logic_processor;

//��Ϣ����ӿ�
class IMessageHandler{
	public:
		virtual bool process_one_msg(queue_msg *data) = 0;//������Ϣ
		
		virtual ~IMessageHandler(){}
	public:
		lobby_logic_processor *lobbyprocess;//����������
};
#endif

