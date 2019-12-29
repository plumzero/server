#ifndef __GATEWAY_SERVER_H__
#define __GATEWAY_SERVER_H__

#include "../include/include.h"
#include "../include/singleton.h"
#include "../manager/servermanager.h"
#include "../message/message_define.h"
#include "../support/tserver.h"
#include "../support/timer.h"

/**
 * 网关服务器保存的客户端信息
 * @param id			对应 ClientInfo 中的 id
 * @param heartbeat		心跳时刻
 * @param client		客户端信息
 */
struct UserClient
{
	CLIENT_ID	id;
	uint64_t	heartbeat;
	ClientInfo* client;
	
	UserClient() :id(0), heartbeat(0), client(nullptr) {}
};

class GatewayServer: public TServer, public Singleton<GatewayServer>
{
public:
	GatewayServer();
	~GatewayServer();
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
public:
	void heart_beat(float delay);
private:
	std::map<CLIENT_ID, UserClient> mUserClients;	// <客户端 id, 客户端信息和心跳>
};


#endif // !__GATEWAY_SERVER_H__