#ifndef __SINGLETON_H__
#define __SINGLETON_H__

// 单例模板定义
template <class T>
class Singleton
{
protected:
	static T* instance;
public:
	static T* getInstance() {
		if (nullptr == instance) {
			instance = new T();
		}
		return instance;
	}
	static T* createInstance() {
		if (nullptr == instance) {
			instance = new T();
		}
		return instance;
	}
	static void destroyInstance() {
		if (instance) {
			delete instance;
			instance = nullptr;
		}
	}
};

// 缺省情况下赋值
template <class T>
T* Singleton<T>::instance = nullptr;

#endif  // ! __SINGLETON_H__
