egg:

```c++
#include"thread_pool.hpp"
void task(void * args)
{
    printf("%s\n",(char*)args);
}

int main()
{
    THREAD_POOL thread_pool(4);//use 4 threads
    thread_pool.throw_task_to_me(task,(void*)"this is args");
}
```

