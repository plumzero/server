#ifndef __FORWARD_SERVER_H__
#define __FORWARD_SERVER_H__

#include "../include/include.h"
#include "../include/singleton.h"
#include "../manager/servermanager.h"
#include "../message/message_define.h"
#include "../support/tserver.h"
#include "../support/timer.h"

class ForwardServer : public TServer, public Singleton<ForwardServer>
{
public:
	ForwardServer();
	~ForwardServer();
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
	void heart_beat(float delay);
private:
	std::map<CLIENT_ID, SERVER_ID> 		mUserGateway;		// <真正客户端id，网关服务器id>		暂时只设置一个网关服务器，id 为 0
	std::map<CLIENT_ID, SERVER_ID> 		mUserBusiness;		// <真正客户端id，业务服务器id>
	std::map<ClientInfo*, uint64_t> 	mPeerHeartBeat;
};

#endif // !__FORWARD_SERVER_H__