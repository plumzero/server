#ifndef __T_SERVER_H__
#define __T_SERVER_H__

#include "../include/include.h"
#include "../manager/servermanager.h"
#include "timer.h"

/**
 * 使服务器具有定时器属性
 * TServer 类以后要被具体的服务器类继承
 * @member mType	服务器类型
 * @member mId		服务器 id
 */
class TServer : public AbstractServer, public Timer
{
public:
	TServer();
	~TServer();
public:
	virtual void net_client_connected(bufferevent* bufev, void* arg);
	virtual void net_client_disconnected(bufferevent* bufev, void* arg);
	virtual void net_client_message_handle(bufferevent* bufev, Message& msg, void* arg);
	virtual void net_server_connected(bufferevent* bufev, void* arg);
	virtual void net_server_disconnected(bufferevent* bufev, void* arg);
	virtual void net_server_message_handle(bufferevent* bufev, Message& msg, void* arg);
public:
	virtual bool srv_init(const char* config);
	virtual void srv_update(float delay);
public:
	SERVER_TYPE get_type() { return mType; }
	SERVER_ID get_id() { return mId; }
protected:
	SERVER_TYPE mType;
	SERVER_ID mId;
};

#endif // !__T_SERVER_H__