
#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_
#include<iostream>
#include<queue>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;
using std::queue;

typedef void(*THREAD_POOL_CALLBACK)(void*args);
typedef struct
{
	THREAD_POOL_CALLBACK func;//任务回调函数
	void* args;
}ROUTINE;

class THREAD_POOL
{
private:
	bool shutdown;
	int max_thread_num;
	int current_thread_num;
	pthread_t *pt;
public:
	queue<ROUTINE> *routine;
	//条件变量和锁
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	THREAD_POOL();
	THREAD_POOL(int thread_num);
	~THREAD_POOL();
	void throw_task_to_me(THREAD_POOL_CALLBACK fun, void*args);
	bool is_destroyed();

};
#endif 
