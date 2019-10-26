#ifndef _ZX_THREAD_H_
#define _ZX_THREAD_H_

/*
Name:           zxThread
Author:         florian.wang
LastModify:     2019/1
Function:       Uint that wrap POSIX thread
*/

/*
#ifdef __ANDROID__
#include "threads.h"
#else
#include <pthread.h>
#endif
*/
#include <pthread.h>

class zxMutex
{
private:
    pthread_mutex_t _mutex;

public:
    zxMutex()
    {
        pthread_mutex_init(&_mutex, NULL);
    }
    ~zxMutex()
    {
        pthread_mutex_destroy(&_mutex);
    }
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    }
    void Unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }
    pthread_mutex_t *get()
    {
        return &_mutex;
    }
    operator pthread_mutex_t *()
    {
        return &_mutex;
    }
};

class zxAutoLock
{
private:
    pthread_mutex_t *_mutex;

public:
    zxAutoLock(pthread_mutex_t *mutex)
    {
        _mutex = mutex;
        pthread_mutex_lock(_mutex);
    }
    ~zxAutoLock()
    {
        pthread_mutex_unlock(_mutex);
    }
};

class zxCond
{
private:
    pthread_cond_t  _cond;

public:
    zxCond()
    {
        pthread_cond_init(&_cond, NULL);
    }
    ~zxCond()
    {
        pthread_cond_destroy(&_cond);
    }
    void wait(pthread_mutex_t *mutex)
    {
        pthread_cond_wait(&_cond, mutex);
    }
    void signal()
    {
        pthread_cond_signal(&_cond);
    }
    void broadcast()
    {
        pthread_cond_broadcast(&_cond);
    }
    pthread_cond_t *get()
    {
        return &_cond;
    }
    operator pthread_cond_t *()
    {
        return &_cond;
    }
};

class zxThread
{
protected:
    pthread_t       _thread;
    zxMutex         _mutex;
    zxCond          _cond;

    bool            _alive;
    char            _name[33];
    void            *_udata;

    static void *thread_func(void *arg);

    zxThread();
    virtual ~zxThread();
    virtual bool run(void *udata) = 0;

public:
    int start(void *udata = NULL, const char *name = NULL);
    int stop();
    int kill();
    int thread_id();
    int wait_finish();
};


#define DEFINE_WORK_THREAD(name, owner, cbfunc) \
class Class_##name: public zxThread\
{\
public:\
    bool run(void *udata)\
    {\
        return ((owner *)udata)->cbfunc();\
    }\
}name;


#endif
