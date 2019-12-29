#include "forwardserver.h"

ForwardServer::ForwardServer() {
	mType = SERVER_TYPE_FORWARD;
	mId	  = SERVER_ID_NONE;
	set_timer(0.5f, &ForwardServer::heart_beat, this);	// 每隔 0.5 s 向其他服务器发送一个心跳包
}

ForwardServer::~ForwardServer() {}
/**
 * 初始化转发器
 * @param config	配置文件的路径
 */
bool ForwardServer::srv_init(const char* config) {
	return true;
}

void ForwardServer::srv_update(float delay) {}
/**
 * 来自其他服务器的连接请求，并向该服务器发送回应，并将该服务器纳入心跳监控。
 * @param bufev		对端服务器对应的 bufferevent
 * @param arg		传递的参数，这里是对端服务器信息(ClientInfo)
 */
void ForwardServer::net_client_connected(bufferevent* bufev, void* arg) {
	ClientInfo* peer = (ClientInfo*)arg;
	
	MSGPP_S2SS_CONNECTED out_msg;
	out_msg.type = static_cast<uint32_t>(mType);
	out_msg.id   = static_cast<uint32_t>(mId);
	
	printf("send S2SS_CONNECTED to slave...\n");
	MsgHeader header(out_msg.type, out_msg.id, 0, 0, S2SS_CONNECTED);
	out_msg.send_message(bufev, header);
	
	mPeerHeartBeat[peer] = 0;
}
/**
 * 来自其他服务器的断开请求，并去除对该服务器的心跳监控。
 * @param bufev	对端服务器对应的 bufferevent
 * @param arg	传递的参数，这里是对端服务器信息(ClientInfo)
 */ 
void ForwardServer::net_client_disconnected(bufferevent* bufev, void* arg) {
	ClientInfo* peer = (ClientInfo*)arg;
	
	printf("peer(%d:%d) disconnected\n", static_cast<uint32_t>(peer->type), static_cast<uint32_t>(peer->id));
	if (mPeerHeartBeat.find(peer) != mPeerHeartBeat.end()) {
		mPeerHeartBeat.erase(peer);
	}
}
/**
 * 处理来自其他服务器的常规消息
 * @param bufev		对端服务器对应的 bufferevent
 * @param msg		来自对端服务器的消息
 * @param arg		传递的参数，这里是对端服务器信息(ClientInfo)
 */
 void ForwardServer::net_client_message_handle(bufferevent* bufev, Message& msg, void* arg) {
	ClientInfo* peer = (ClientInfo*)arg;
	switch (msg.header.msg_id) {
		case SS2S_HEARTBEAT:
		{
			printf("receive SS2S_HEARTBEAT from slave...\n");
			mPeerHeartBeat[peer] = get_current_time();
			break;
		}
		case SS2S_CONNECTED:
		{
			printf("receive SS2S_CONNECTED from slave...\n");
			MSGPP_SS2S_CONNECTED in_msg(msg.payload);
			peer->type = static_cast<CLIENT_TYPE>(in_msg.type);
			peer->id   = in_msg.id;
			
			printf("accept peer(%d:%d) connected\n", in_msg.type, in_msg.id);
			break;
		}
		default:
		{
			if (msg.header.from_type == static_cast<uint32_t>(SERVER_TYPE_CLIENT)) {
				if (msg.header.to_type == static_cast<uint32_t>(SERVER_TYPE_BUSINESS)) {
					if (mUserBusiness.find(msg.header.from_id) != mUserBusiness.end()) {
						SERVER_ID business_id = mUserBusiness[msg.header.from_id];
						ServerManager::getInstance()->srvmgr_send_msg_to_client(msg, SERVER_TYPE_BUSINESS, business_id);
					}
				} else {
					ServerManager::getInstance()->srvmgr_send_msg_to_client(msg, static_cast<SERVER_TYPE>(msg.header.to_type), static_cast<SERVER_ID>(msg.header.to_id));
				}
				mUserGateway[msg.header.from_id] = static_cast<SERVER_ID>(peer->id);
			} else if (msg.header.to_type == static_cast<uint32_t>(SERVER_TYPE_CLIENT)) {
				CLIENT_ID user_id = msg.header.to_id;
				if (mUserGateway.find(user_id) != mUserGateway.end()) {
					SERVER_ID gateway_id = mUserGateway[user_id];
					ServerManager::getInstance()->srvmgr_send_msg_to_client(msg, SERVER_TYPE_GATEWAY, gateway_id);
				}
				if (msg.header.from_type == static_cast<uint32_t>(SERVER_TYPE_BUSINESS)) {
					mUserBusiness[msg.header.to_id] = static_cast<SERVER_ID>(msg.header.from_id);
				}
			} else {
				ServerManager::getInstance()->srvmgr_send_msg_to_client(msg, static_cast<SERVER_TYPE>(msg.header.to_type), static_cast<SERVER_ID>(msg.header.to_id));
			}
			break;
		}
	}
}

void ForwardServer::net_server_connected(bufferevent* bufev, void* arg) {}

void ForwardServer::net_server_disconnected(bufferevent* bufev, void* arg) {}

void ForwardServer::net_server_message_handle(bufferevent* bufev, Message& msg, void* arg) {}

void ForwardServer::heart_beat(float delay) {
	uint64_t cur_time = get_current_time();
	
	for (std::map<ClientInfo*, uint64_t>::iterator it = mPeerHeartBeat.begin(); it != mPeerHeartBeat.end(); it++) {
		if (it->second && cur_time - it->second > 10000) {
			it->first->events = 0;
		}
	}
	
	MSGPP_S2SS_HEARTBEAT out_msg;
	out_msg.cur_time = cur_time;
	
	MsgHeader header(S2SS_HEARTBEAT);
	out_msg.broadcast_msg_to_client(header);
}
