#include "gatewayserver.h"

#define VerifyClient	\
	(client->id == msg.header.from_id && mUserClients[client->id].id && mUserClients.find(client->id) != mUserClients.end() && mUserClients[client->id].client == client)

GatewayServer::GatewayServer() {
	mType = SERVER_TYPE_GATEWAY;
	mId   = SERVER_GATEWAY_SYNCHRON;
	
	set_timer(1.0f, &GatewayServer::heart_beat, this);
}

GatewayServer::~GatewayServer() {
	kill_timer("heartbeat");
}

bool GatewayServer::srv_init(const char* config) {
	return true;
}

void GatewayServer::srv_update(float delay) {}

void GatewayServer::net_client_connected(bufferevent* bufev, void* arg) {}
/**
 * 网关与客户端断开，将网关中保存的客户端信息删除
 * @param bufev		客户端的 bufev
 * @param arg		传递的参数，这里是客户端信息(ClientInfo)
 */
void GatewayServer::net_client_disconnected(bufferevent* bufev, void* arg) {
	ClientInfo* client = (ClientInfo*)arg;
	
	if (mUserClients.find(client->id) != mUserClients.end()) {
		mUserClients.erase(client->id);
	}
}
/**
 * 处理来自客户端的消息
 * @param bufev	客户端的 bufferevent
 * @param msg	客户端的消息
 * @param arg	传递的参数，这里是客户端的信息(ClientInfo)
 */
void GatewayServer::net_client_message_handle(bufferevent* bufev, Message& msg, void* arg) {
	ClientInfo* client = (ClientInfo*)arg;
	printf("receive Message from client...\n");
	switch (msg.header.msg_id) {
		case C2GS_HEARTBEAT:
		{
			printf("receive C2GS_HEARTBEAT from client...\n");
			if (mUserClients.find(client->id) != mUserClients.end()) {
				mUserClients[client->id].heartbeat = get_current_time();
			}
			break;
		}
		case C2DS_REQUEST_LOGIN:
		{
			printf("receive C2DS_REQUEST_LOGIN from client...\n");
			if (client->type && client->id && msg.header.from_id != client->id) {
				break;
			}
			client->type = static_cast<CLIENT_TYPE>(msg.header.from_type);
			client->id   = msg.header.from_id;				
			mUserClients[client->id].client = client;
			
			msg.header.to_type = static_cast<uint32_t>(SERVER_TYPE_DATABASE);
			msg.header.to_id   = static_cast<uint32_t>(SERVER_DATABASE_MYSQL);
			
			ServerManager::getInstance()->srvmgr_send_msg_to_server(msg, SERVER_TYPE_FORWARD, SERVER_ID_NONE);
			break;
		}
		case C2DS_REQUEST_LOGOUT:
		case C2DS_GET_XXX:
		case C2DS_SET_XXX:
		{
			if (VerifyClient) {
				msg.header.to_type = static_cast<uint32_t>(SERVER_TYPE_DATABASE);
				msg.header.to_id   = static_cast<uint32_t>(SERVER_DATABASE_MYSQL);
				
				ServerManager::getInstance()->srvmgr_send_msg_to_server(msg, SERVER_TYPE_FORWARD, SERVER_ID_NONE);
			}
			break;
		}
		case C2BS_GET_XXX:
		case C2BS_SET_XXX:
		{
			if (VerifyClient) {
				msg.header.to_type = static_cast<uint32_t>(SERVER_TYPE_BUSINESS);
				msg.header.to_id   = static_cast<uint32_t>(SERVER_BUSINESS_FIRST);
				
				ServerManager::getInstance()->srvmgr_send_msg_to_server(msg, SERVER_TYPE_FORWARD, SERVER_ID_NONE);
			}
			break;
		}
		default:
			break;
	}
}

void GatewayServer::net_server_connected(bufferevent* bufev, void* arg) {}

void GatewayServer::net_server_disconnected(bufferevent* bufev, void* arg) {}

/**
 * 处理来自转发服务器的消息
 * @param bufev		与转发器交互的 bufferevent
 * @param msg		收发的消息
 * @param arg		传递的参数，这里是连接的对端信息(ConnectInfo)
 */
void GatewayServer::net_server_message_handle(bufferevent* bufev, Message& msg, void* arg) {
	ConnectInfo* peer = (ConnectInfo*)arg;
	
	switch(msg.header.msg_id) {
		case S2SS_HEARTBEAT:
		{
			printf("receive S2SS_HEARTBEAT from forward...\n");
			MSGPP_S2SS_HEARTBEAT in_msg(msg.payload);
			
			MSGPP_SS2S_HEARTBEAT out_msg;
			out_msg.cur_time = in_msg.cur_time;
			
			MsgHeader header(SS2S_HEARTBEAT);
			out_msg.send_message(bufev, header);
			break;
		}
		case S2SS_CONNECTED:
		{
			printf("receive S2SS_CONNECTED from forward...\n");
			MSGPP_S2SS_CONNECTED in_msg(msg.payload);
			peer->type = static_cast<SERVER_TYPE>(in_msg.type);
			peer->id   = static_cast<SERVER_ID>(in_msg.id);
			
			MSGPP_SS2S_CONNECTED out_msg;
			out_msg.type = static_cast<uint32_t>(mType);
			out_msg.id   = static_cast<uint32_t>(mId);
			
			MsgHeader header(SS2S_CONNECTED);
			out_msg.send_message(bufev, header);
			break;
		}
		case DS2C_REQUEST_LOGIN:
		{
			MSGPP_DS2C_REQUEST_LOGIN in_msg(msg.payload);
			if (in_msg.error_code == 0) {
				CLIENT_ID user_id = msg.header.to_id;
				if (mUserClients.find(user_id) != mUserClients.end()) {
					ClientInfo* client = mUserClients[user_id].client;
					if (client->id == user_id) {
						mUserClients[user_id].id = user_id;
						
						ServerManager::getInstance()->srvmgr_send_message(client->bufev, msg);
					}
					printf("UserLogin: %u %s:%u\n", user_id, client->host.c_str(), client->port);
				}
			}
			break;
		}
		default:
		{
			if (static_cast<uint32_t>(SERVER_TYPE_CLIENT) == msg.header.to_type) {
				uint32_t user_id = msg.header.to_id;
				if (mUserClients.find(user_id) != mUserClients.end()) {
					ServerManager::getInstance()->srvmgr_send_message(mUserClients[user_id].client->bufev, msg);
				}
			}
			break;
		}
	}
}

void GatewayServer::heart_beat(float delay) {
	printf("%s %d heart beat to client\n", __FILE__, __LINE__);
	uint64_t cur_time = get_current_time();
	for (std::map<CLIENT_ID, UserClient>::iterator it = mUserClients.begin(); it != mUserClients.end(); it++) {
		if (it->second.heartbeat && cur_time - it->second.heartbeat > 30000) {
			it->second.client->events = 0;
			printf("TimeOut:%u %s %u\n", it->second.id, it->second.client->host.c_str(), it->second.client->port);
		}
	}
	
	MSGPP_GS2C_HEARTBEAT out_msg;
	out_msg.cur_time = cur_time;
	
	MsgHeader header(GS2C_HEARTBEAT);
	
	for (std::map<CLIENT_ID, UserClient>::iterator it = mUserClients.begin(); it != mUserClients.end(); it++) {
		if (it->second.id) {
			out_msg.send_message(it->second.client->bufev, header);
		}
	}
}