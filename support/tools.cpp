#include "tools.h"

//////////////////// 宏工具 ////////////////////
#define ISBLANK(x)  (uint32_t)((((unsigned char)x) == ' ') || (((unsigned char)x) == '\t'))

//////////////////// 普通工具 ////////////////////
// 获取当前时间（毫秒计）
uint64_t get_current_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
// 获取以Epoch点作为统计的现在时间
struct timeval tv_now(void) {
	struct timeval now;
	now.tv_sec = static_cast<uint32_t>(time(NULL));
	now.tv_usec = 0;
	return now;
}
// 计算两个时间点的间隔
double tv_diff_secs(struct timeval newer, struct timeval older) {
	if(newer.tv_sec != older.tv_sec)
		return static_cast<double>(newer.tv_sec-older.tv_sec) +
				static_cast<double>(newer.tv_usec-older.tv_usec) / 1000000.0;
	else
		return static_cast<double>(newer.tv_usec-older.tv_usec) / 1000000.0;
}
// 休眠函数，单位毫秒
void go_sleep(uint32_t ms) {
#if defined(WIN32)
	Sleep(ms);
#else
	struct timeval timeout;
	timeout.tv_sec = ms / 1000L;
	ms = ms % 1000L;
	timeout.tv_usec = static_cast<uint32_t>(ms * 1000);
	select(0, NULL,  NULL, NULL, &timeout);
#endif
}