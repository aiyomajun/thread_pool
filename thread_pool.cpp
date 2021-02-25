#include"thread_pool.hpp"

void *thread_pool_thread_entry(void*args)
{
	THREAD_POOL *_this = (THREAD_POOL*)args;
	//printf("%ld _this=\n", _this);
	while (1)
	{
		pthread_mutex_lock(&_this->mutex);
		while (_this->routine->empty()) 
		{
			pthread_cond_wait(&_this->cond, &_this->mutex);
		}
		if (_this->is_destroyed())//线程池被销毁了
		{
			pthread_exit((void*)"thread poll is destroyed");
		}

		ROUTINE r = _this->routine->front();
		_this->routine->pop();
		pthread_mutex_unlock(&_this->mutex);
		THREAD_POOL_CALLBACK fun = r.func;
		//printf("I get a task now run it%ld\n",pthread_self());
		fun(r.args);
		
	}

}

THREAD_POOL::THREAD_POOL()
{

	routine = new queue<ROUTINE>();
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	current_thread_num = 8;
	max_thread_num = 8*2;
	shutdown = false;
	int i;
	pt = (pthread_t*)malloc(sizeof(pthread_t) * 8);
	for (i = 0; i < 8; i++)
	{
		pthread_create(&pt[i], NULL, thread_pool_thread_entry,this);
	}
}
THREAD_POOL::THREAD_POOL(int thread_num)
{
	routine = new queue<ROUTINE>();
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	current_thread_num = thread_num;
	max_thread_num = thread_num*2;
	shutdown = false;
	int i;
	pt = (pthread_t*)malloc(sizeof(pthread_t) * thread_num);
	for (i = 0; i < thread_num; i++)
	{
		pthread_create(&pt[i], NULL, thread_pool_thread_entry, this);
	}
}

THREAD_POOL::~THREAD_POOL()
{
	shutdown = true;
	pthread_cond_broadcast(&cond);


	pthread_mutex_lock(&mutex);

	while (!routine->empty())
	{
		routine->pop();
	}

	pthread_mutex_unlock(&mutex);
	void **ret;
	for (int i = 0; i < current_thread_num; i++)
	{
		pthread_join(pt[i], ret);
	}
	delete routine;
	free(pt);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}

void THREAD_POOL::throw_task_to_me(THREAD_POOL_CALLBACK fun, void*args)
{
	ROUTINE r;
	r.func = fun;
	r.args = args;
	//printf("in throw_task_to_me\n");
	pthread_mutex_lock(&mutex);
	//printf("in throw_task_to_me,got mutex\n");
	routine->push(r);
	//printf("in throw_task_to_me,rel mutex\n");

	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);//唤醒一个线程
	
	
	//printf("in throw_task_to_me,signal cond\n");
}


bool THREAD_POOL::is_destroyed()
{
	return shutdown;
}

