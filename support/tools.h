#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "../include/include.h"

// 普通工具
uint64_t get_current_time();
struct timeval tv_now(void);
double tv_diff_secs(struct timeval newer, struct timeval older);
void go_sleep(uint32_t ms);

// 模板工具
template <typename itype, typename otype>
static void convert(itype input, otype& output) {
	std::stringstream ss;
	ss << input;
	ss >> output;
}

template <typename itype, typename otype>
static bool convertstr(itype input, otype& output) {	// convert 解析字符串时有遇空白符读断问题，新增一个专门解析字符串的模板函数
	char ibuf[BUF1KSIZE], obuf[BUF1KSIZE];
	size_t i, len;
	std::stringstream ss;	
	ss << input;
	len = ss.str().length();
	memset(ibuf, 0, BUF1KSIZE);
	memset(obuf, 0, BUF1KSIZE);
	std::strcpy(ibuf, ss.str().c_str());
	for (i = 0; i < len && i < BUF1KSIZE; i++) {
		if (ibuf[i] == 32 || ibuf[i] == 9) {	// 32 - 空格符	9 - tab 符
			obuf[i] = ibuf[i];
			ss.seekg(i + 1);
		} else {
			ss >> obuf[i];
		}
	}
	if (i == BUF1KSIZE) {		// 截断字符串
		obuf[BUF1KSIZE - 1] = '\0';
	}
	output = obuf;
	return true;
}

template <typename otype, typename itype>
static otype get_value(itype input) {
	otype output;
	convert(input, output);
	return output;
}

// 失败的函数
template <typename otype, typename itype>
static otype get_str(itype input) {
	otype output;
	convertstr(input, (std::string&)output);
	return output;
}

template <typename T>
static std::string get_string(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

#endif // !__TOOLS_H__
