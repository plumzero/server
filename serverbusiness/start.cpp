#include "../include/include.h"
#include "../manager/mysqlmanager.h"
#include "businessserver.h"

void signal_cb(evutil_socket_t fd, short events, void* arg) {
	struct event_base* base = (struct event_base*)arg;
	struct timeval delay = {2, 0};
	
	fprintf(stderr, "Caught an interrupt signal, exit cleanly in %d seconds.\n", (int)delay.tv_sec);
	
	event_base_loopexit(base, &delay);
}

// 暂只对 linux 系统有效
int main(int argc, char* argv[]) {
	
	const char* config = nullptr;
	struct event_base* base = nullptr;
	struct event* signal_event = nullptr;
	
	config = argc > 1 ? argv[1] : "business.conf";
	char configpath[PATH_MAX];
	if (nullptr == realpath(config, configpath))
		return -1;
	
	if ((base = event_base_new()) == NULL) {
		fprintf(stderr, "%s %d event_base_new failed\n", __FILE__, __LINE__);
		return -1;
	}
	
	Timer::set_base(base);
	ServerManager::set_base(base);
	ServerManager::getInstance()->srvmgr_init(configpath);
	SqlManager::getInstance()->sqlmgr_init(configpath);
	BusinessServer::getInstance()->srv_init(configpath);
	
	if ((signal_event = evsignal_new(base, SIGINT, signal_cb, (void*)base)) == NULL) {
		fprintf(stderr, "%s %d evsignal_new failed\n", __FILE__, __LINE__);
		return -1;
	}
	if (0 != event_add(signal_event, nullptr)) {
		fprintf(stderr, "%s %d event_add failed\n", __FILE__, __LINE__);
		return -1;
	}
	
	event_base_dispatch(base);
	event_base_free(base);
	
	return 0;
}
