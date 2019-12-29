#include "timer.h"

// 一次性超时回调
void timer_once_cb(evutil_socket_t fd, short events, void* args) {
	TimerInfo* timer = (TimerInfo *)args;
	
	if (timer && timer->callback) {
		timer->callback(get_current_time() - timer->curtime / 1000.0);
		delete timer;
	}
}
// 持久性超时回调
void timer_event_cb(evutil_socket_t fd, short events, void* args) {
	TimerInfo* timer = (TimerInfo*)args;
	
	if (timer && timer->callback) {
		uint64_t last_time		= timer->curtime;
		uint64_t now_time		= get_current_time();
		timer->callback((now_time - last_time) / 1000.0f);
		timer->curtime = now_time;
	}
}
// 类成员定义
event_base* Timer::mBase = nullptr;

Timer::Timer() { }

Timer::~Timer() {
	kill_all_timers();
}

/**
 * 设置定时器事件为一次性事件，一次性事件被激活后成为非未决状态
 * 一次性定时器不保存
 */
void Timer::set_timer_once(float delay, const timer_func_ptr& callback) {
	TimerInfo* timer 	= new TimerInfo();
	timer->callback 	= callback;
	timer->curtime 		= get_current_time();
	
	time_t sec 			= delay;
	suseconds_t usec 	= (delay - sec) * 1000000;
	struct timeval tv 	= { sec, usec };
	
	if (0 != event_base_once(mBase, -1, EV_TIMEOUT, timer_once_cb, timer, &tv)) {
		fprintf(stderr, "%s %s %d event_base_once failed\n", __FILE__, __FUNCTION__, __LINE__);
		return;
	}
}

/**
 * 设置定时器为持久(EV_PERSIST)事件，回调被激活后，事件还是会保持为未决状态
 * 定时器保存入计时器列表
 */
void Timer::set_timer(void* method, float interval, const timer_func_ptr& callback) {
	TimerInfo* timer	= new TimerInfo();
	timer->callback		= callback;
	timer->method		= method;
	timer->curtime		= get_current_time();
	mTimers.push_back(timer);
	
	struct event* timer_event = event_new(mBase, -1, EV_PERSIST, timer_event_cb, timer);
	timer->timer 		= timer_event;
	
	time_t sec 			= interval;
	suseconds_t usec 	= (interval - sec) * 1000000;
	struct timeval tv	= { sec, usec };
	event_add(timer_event, &tv);
}

void Timer::set_timer(std::string key, float interval, const timer_func_ptr& callback) {
	TimerInfo* timer	= new TimerInfo();
	timer->callback		= callback;
	timer->key			= key;
	timer->curtime		= get_current_time();
	mTimers.push_back(timer);
	
	struct event* timer_event = event_new(mBase, -1, EV_PERSIST, timer_event_cb, timer);
	timer->timer		= timer_event;
	
	time_t sec			= interval;
	suseconds_t usec	= (interval - sec) * 1000000;
	struct timeval tv	= { sec, usec };
	event_add(timer_event, &tv);
}

void Timer::kill_timer(const std::string key) {
	for (std::list<TimerInfo*>::iterator it = mTimers.begin(); it != mTimers.end(); ) {
		TimerInfo* timer = *it;
		if (timer->key == key) {
			delete timer;
			mTimers.erase(it++);
		} else {
			it++;
		}
	}
}

void Timer::kill_all_timers() {
	for (std::list<TimerInfo*>::iterator it = mTimers.begin(); it != mTimers.end(); it++) {
		delete (*it);
	}
	mTimers.clear();
}
