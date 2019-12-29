
#include "test_client.h"

//////////////////// 测试用工具函数 ////////////////////

#define Location	printf("%s %d ********************\n", __FUNCTION__, __LINE__)

// signal(SIGPIPE, SIG_IGN);

ssize_t writen(int fd, unsigned char* vptr, size_t n)
{
	size_t			nleft;
	ssize_t			nwritten;
	unsigned char*	ptr;
	
	ptr = vptr;
	nleft = n;
	while (nleft > 0){
		if ((nwritten = write(fd, ptr, nleft)) <= 0){
			if (errno == EINTR)
				nwritten = 0;		/* 重新调用 */
			else
				return -1;			/* 出错 */
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

//////////////////// Socket 类 ////////////////////

Socket::Socket(uint32_t id){
	mId = id;
	mSocket = -1;
	mBuffer = new unsigned char[BUF2KSIZE];
	mBufsize = BUF2KSIZE;
};

Socket::~Socket() {
	if (mBuffer) { delete [] mBuffer; mBuffer = nullptr; };
};

bool Socket::skt_read_buf(std::string& msg, unsigned char* data, size_t datalen) {
	if (datalen > PROHEADERSIZE) {
		ProtocolHeader header;
		memcpy(&header, data, datalen);
		uint8_t code = ~header.orthocode;
		uint16_t bodysize = ntohs(header.bodysize);
		
		if (datalen >= PROHEADERSIZE + bodysize) {
			data += PROHEADERSIZE;
			unsigned char* sbuf = new unsigned char[bodysize];
			memcpy(sbuf, data, bodysize);
			
			std::string strdata = std::string((const char*)sbuf, bodysize);
			delete[] sbuf;
			
			if (strdata.size() == bodysize) {
				size_t i;
				for (i = 0; i < strdata.size(); i++) {
					// strdata[i] ^= code;
					msg = strdata;
					return true;
				}
			}
		}
	}
	
	return false;
}

bool Socket::skt_write_buf(std::string msg, unsigned char* buf, size_t buflen) {
	ProtocolHeader header;
	header.version = 1;
	header.orthocode = rand() % 0xFF;
	header.bodysize = htons(msg.size());
	
	uint8_t code = ~header.orthocode;
	size_t i;
	for (i = 0; i < msg.size(); i++) {
		// msg[i] ^= code;
	}
	std::string oput = std::string((const char*)&header, PROHEADERSIZE) + msg;
	assert(buflen >= oput.size());		// 低效，去掉
	size_t olen = oput.size();
	printf("olen -------%lu\n", olen);
	memcpy(buf, oput.c_str(), olen);
	
	ssize_t n = write(mSocket, buf, olen);
	
	return (size_t)n == olen;	
}
	
bool Socket::skt_create() {
	if ((mSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "%d ecode(%08X): %s\n", __LINE__, errno, strerror(errno));
		exit(-1);
	}
	skt_setnoblocking();
	return true;
}

uint32_t Socket::skt_get() {
	if (mSocket > 0) {
		return mSocket;
	}
	skt_create();
	
	return mSocket;
}

void Socket::skt_setnoblocking() {
	uint32_t opts = 0;
	opts = fcntl(mSocket, F_GETFL);
	opts = opts | O_NONBLOCK;
	fcntl(mSocket, F_SETFL);
}
	
bool Socket::skt_connect(std::string host, uint16_t port) {
	struct sockaddr_in sin;
	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(host.c_str());
	sin.sin_port = htons(port);
	if (connect(mSocket, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
		fprintf(stderr, "socket %d connected failed\n", mSocket);
		return false;
	}
	return true;
}

bool Socket::skt_login() {
	MsgHeader header(SERVER_TYPE_CLIENT, mId, SERVER_TYPE_GATEWAY, SERVER_GATEWAY_SYNCHRON, C2DS_REQUEST_LOGIN);
	// 建立身价
	MSGPP_C2DS_REQUEST_LOGIN msgpp_out;
	msgpp_out.id = mId;
	char buf[32] = {0};
	snprintf(buf, 32, "%d", mId);
	msgpp_out.name = "name" + std::string(buf);
	msgpp_out.password = "password" + std::string(buf);
	Message message;
	message.header = header;
	message.payload = msgpp_out.get_value();
	memset(mBuffer, 0, mBufsize);
	std::string smsg = message.get_string();
	skt_write_buf(smsg, mBuffer, mBufsize);
	
	return true;
}

void Socket::skt_message_dispatch(struct timeval tv) {
	if (mBuffer == nullptr) {
		return ;
	}
	uint32_t tvflag = 0;
	struct timeval tvnow, tvold;
	ssize_t ret, n;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(mSocket, &rfds);
	printf("%d mSocket = %d\n", __LINE__, mSocket);
	for (;;) {
		ret = select(mSocket + 1, &rfds, NULL, NULL, &tv);
		printf("%d ret = %d\n", __LINE__, (int)ret);
		// go_sleep(1000);
		if (FD_ISSET(mSocket, &rfds)) {
			if (ret == -1) {
				fprintf(stderr, "%d ecode(%08X): %s\n", __LINE__, errno, strerror(errno));
				return ;
			} else if (ret == 0) {
				Location;
				printf("the remote server does not have message to send you or it may be drop anchor\n");
				tvnow = tv_now();
				if (!tvflag) {
					tvold = tvnow;
					tvflag = 1;
				}
				if (tv_diff_secs(tvnow, tvold) < 5)	// 如果服务器抛锚，客户端在 5s 后与其断开
					continue;
				break;
				
			} else {
				Location;
				tvflag = 0;
				memset(mBuffer, 0, mBufsize);
				n = read(mSocket, mBuffer, mBufsize);
				if (n == -1) {
					fprintf(stderr, "%d ecode(%08X): %s\n", __LINE__, errno, strerror(errno));
					return ;
				} else if (n == 0) {
					printf("Connect closed or read over.\n");
					return ;
				} else {
					skt_server_message_handle(mBuffer, (size_t)n);
				}
			}
		} else {
			Location;
			FD_SET(mSocket, &rfds);
			continue;
		}
	}
}

void Socket::skt_server_message_handle(unsigned char* data, size_t datalen) {
	std::string str_msg;
	size_t rdsize = 0;
	if (!skt_read_buf(str_msg, data, datalen))
		return ;
	if (str_msg.empty() || datalen - PROHEADERSIZE < rdsize) {
		fprintf(stderr, "%d read message failed(%d != %d)\n", __LINE__, (uint32_t)rdsize, (uint32_t)(datalen - PROHEADERSIZE));
		return ;
	}
	Message msg(str_msg);
	switch(msg.header.msg_id) {
		case GS2C_HEARTBEAT:
		{
			printf("receive GS2C_HEARTBEAT from forward...\n");
			MSGPP_GS2C_HEARTBEAT msgpp_in(msg.payload);
			
			MsgHeader header(SERVER_TYPE_CLIENT, mId, SERVER_TYPE_GATEWAY, SERVER_GATEWAY_SYNCHRON, C2GS_HEARTBEAT);
			Message message;
			message.header = header;
			message.payload = msgpp_in.get_value();
			
			memset(mBuffer, 0, mBufsize);
			skt_write_buf(message.get_string(), mBuffer, mBufsize);
						
			break;
		}
		default:
			break;
	}
}

void Socket::skt_client_message_handle(unsigned char* data, size_t datalen) {
	
}