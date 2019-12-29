#ifndef __INCLUDE_H__
#define __INCLUDE_H__

// 对 C 库的引用
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <ctime>
#include <cerrno>
#include <cassert>

// C++ 基本库
#include <iomanip>

// 流操作
#include <sstream>
#include <fstream>

// 容器操作
#include <vector>
#include <list>
#include <map>
#include <set>

// 容器算法
#include <algorithm>

// boost 库
#include <mutex>
#include <condition_variable>
#include <thread>

// libevent
#include <event.h>
// #include <evhttp.h>
// #include <event2/event.h>
#include <event2/listener.h>

// json
#include <json/json.h>

// mysql
#include <mysql/mysql.h>

// linux 环境下
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF16SIZE		16
#define BUF32SIZE		32
#define BUF64SIZE		64
#define BUF12SIZE		128
#define BUF25SIZE		256
#define BUF51SIZE		512
#define BUF1KSIZE		1024
#define BUF2KSIZE		2048
#define BUF4KSIZE		4096

/**
 * 自定义协议头
 * @member	version		版本号
 * @member  orthocode	正交码
 * @member  bodysize	消息体长
 * @member  placeholder 占位符，以后可作校验码用
 */
struct ProtocolHeader
{
	uint8_t 	version;
	uint8_t 	orthocode;
	uint16_t	bodysize;
	uint32_t	placeholder;
	
	ProtocolHeader() :version(0), orthocode(0), bodysize(0), placeholder(0) {}
};

// 服务器类型（客户端暂以 0 标识）
typedef enum {
	SERVER_TYPE_NONE			= 0 << 0,
	SERVER_TYPE_CLIENT			= 1 << 0,	// 客户端
	SERVER_TYPE_FORWARD			= 1 << 1,	// 转发器
	SERVER_TYPE_GATEWAY			= 1 << 2,	// 网关
	SERVER_TYPE_DATABASE		= 1 << 3,	// 数据库
	SERVER_TYPE_BUSINESS		= 1 << 4,	// 业务
	
	SERVER_TYPE_ALL				= -1,
} SERVER_TYPE;

typedef SERVER_TYPE		CLIENT_TYPE;
typedef uint32_t 		CLIENT_ID;

typedef enum {
	SERVER_ID_NONE						= 0 << 0,
	// 网关服务器ID
	SERVER_GATEWAY_SYNCHRON				= 1 << 0,	// 同调士
	SERVER_GATEWAY_RESONATOR			= 1 << 1,	// 共鸣者
	SERVER_GATEWAY_CHRONOMALY			= 1 << 2,	// 先史遗产
	SERVER_GATEWAY_MADOLCHE				= 1 << 3, 	// 魔偶甜点
	SERVER_GATEWAY_LIGHTSWORN			= 1 << 4,	// 光道
	SERVER_GATEWAY_INFERNITY			= 1 << 5,	// 永火
	SERVER_GATEWAY_NEKROZ				= 1 << 6,	// 影灵依
	SERVER_GATEWAY_INFERNOID			= 1 << 7,	// 狱火机
	SERVER_GATEWAY_BROTFIREFIST			= 1 << 8,	// 炎星
	SERVER_GATEWAY_BUJIN				= 1 << 9,	// 武神
	// 数据库服务器ID
	SERVER_DATABASE_MYSQL				= 1 << 11,
	SERVER_DATABASE_PGSQL				= 1 << 12,
	SERVER_DATABASE_SQLITE				= 1 << 13,
	SERVER_DATABASE_REDIS				= 1 << 14,
	SERVER_DATABASE_MOONGO				= 1 << 15,
	SERVER_DATABASE_MEMCACHED			= 1 << 16,
	// 业务服务器ID
	SERVER_BUSINESS_FIRST				= 1 << 20,
	SERVER_BUSINESS_SECOND				= 1 << 21,
	SERVER_BUSINESS_THIRD				= 1 << 22,
	
	// 所有服务器
	SERVER_ID_ALL						= -1,
} SERVER_ID;

#endif  // !__INCLUDE_H__