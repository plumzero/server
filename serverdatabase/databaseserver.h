#ifndef __DATABASE_SERVER_H__
#define __DATABASE_SERVER_H__

#include "../include/include.h"
#include "../include/singleton.h"
#include "../manager/servermanager.h"
#include "../message/message_define.h"
#include "../support/tserver.h"
#include "../support/timer.h"

class DatabaseServer: public TServer, public Singleton<DatabaseServer>
{
public:
	DatabaseServer();
	~DatabaseServer();
public:
	virtual bool srv_init(const char* config);
	virtual void srv_update(float delay);
public:
	virtual void net_client_connected(bufferevent* bufev, void* arg);
	virtual void net_client_disconnected(bufferevent* bufev, void* arg);
	virtual void net_client_message_handle(bufferevent* bufev, Message& msg, void* arg);
	virtual void net_server_connected(bufferevent* bufev, void* arg);
	virtual void net_server_disconnected(bufferevent* bufev, void* arg);
	virtual void net_server_message_handle(bufferevent* bufev, Message& msg, void* arg);
};

#endif 	// !__DATABASE_SERVER_H__