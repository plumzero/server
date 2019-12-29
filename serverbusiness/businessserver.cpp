#include "businessserver.h"

BusinessServer::BusinessServer() {
	mType = SERVER_TYPE_BUSINESS;
	mId   = SERVER_BUSINESS_FIRST;
}

BusinessServer::~BusinessServer() {}

bool BusinessServer::srv_init(const char* config) {
	return true;
}

void BusinessServer::srv_update(float delay) {}

void BusinessServer::net_client_connected(bufferevent* bufev, void* arg) {}

void BusinessServer::net_client_disconnected(bufferevent* bufev, void* arg) {}

void BusinessServer::net_client_message_handle(bufferevent* bufev, Message& msg, void* arg) {}

void BusinessServer::net_server_connected(bufferevent* bufev, void* arg) {}

void BusinessServer::net_server_disconnected(bufferevent* bufev, void* arg) {}

void BusinessServer::net_server_message_handle(bufferevent* bufev, Message& msg, void* arg) {
	ConnectInfo* peer = (ConnectInfo*)arg;
	
	switch (msg.header.msg_id) {
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
		// 添加具体的消息处理
		default:
			break;
	}
}
