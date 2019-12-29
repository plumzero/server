#ifndef __TIMER_H__
#define __TIMER_H__

#include "../include/include.h"
#include "../support/tools.h"

// 为每个服务器设置一个定时器

typedef std::function<void(float)> timer_func_ptr;

struct TimerInfo
{
	struct event* 	timer;
	std::string 	key;
	void* 			method;
	timer_func_ptr 	callback;
	uint64_t 		curtime;
	
	TimerInfo() : timer(nullptr), method(nullptr) {}
	~TimerInfo() { if (timer) { event_free(timer); } }
};

/**
 * set_timer_once
 * 时间超时后，如果没有动作，会且仅且调用此计时器一次。
 * set_timer_once 有两个重载函数，一个常规函数，一个是模板函数。
 * 模板函数调用的是类obj的方法
 * @param delay					超时时间
 * @param callback(method) 		超时时间的回调函数
 * @param args					回调函数的参数(未使用)
 *
 * set_timer
 * 时间超时后，如果没有动作，会调用此计时器
 * @param delay(interval)		超时间隔
 * @param method(callback)		超时时间的回调函数
 * @param key					回调函数对应的键，通过此键确定应该调用的超时回调
 * @param args					回调函数的参数(未使用)
 */
class Timer
{
public:
	Timer();
	~Timer();
public:
	void set_timer_once(float delay, const timer_func_ptr& callback);
	template<typename T, typename ...V, typename ...Args>
	void set_timer_once(float delay, void (T::* method)(float, V...), T* obj, Args...args);
	
	void set_timer(void* method, float interval, const timer_func_ptr& callback);
	void set_timer(std::string key, float interval, const timer_func_ptr& callback);
	template<typename T, typename ...V, typename ...Args>
	void set_timer(float delay, void (T::* method)(float, V...), T* obj, Args...args);
	
	void kill_timer(const std::string key);
	template<typename T, typename ...V>
	void kill_timer(void (T::* method)(float, V...));
	
	void kill_all_timers();
public:
	static void set_base(event_base* base) { mBase = base; }
private:
	static event_base* 			mBase;
	std::list<TimerInfo*> 		mTimers;
};

/**
 * Timer::set_timer_once
 * callback 引用的类 obj 的 method 方法，参数是 std::placeholders::_1 和 args...
 */
template<typename T, typename ...V, typename ...Args>
void Timer::set_timer_once(float delay, void (T::* method)(float, V...), T* obj, Args...args) {
	timer_func_ptr callback = std::bind(method, obj, std::placeholders::_1, args...);
	set_timer_once(delay, callback);
}

template<typename T, typename ...V, typename ...Args>
void Timer::set_timer(float delay, void (T::* method)(float, V...), T* obj, Args...args) {
	timer_func_ptr callback = std::bind(method, obj, std::placeholders::_1, args...);
	set_timer(*(void**)&method, delay, callback);
}

template<typename T, typename ...V>
void Timer::kill_timer(void (T::* method)(float, V...)) {
	for (std::list<TimerInfo*>::iterator it = mTimers.begin(); it != mTimers.end(); ) {
		TimerInfo* timer = *it;
		if (timer->method == *(void**)&method) {
			delete timer;
		} else {
			it++;
		}
	}
}

#endif // !__TIMER_H__