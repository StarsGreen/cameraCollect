#ifndef _ZX_QUEUE_H_
#define _ZX_QUEUE_H_

/*
Name:           zxQueue
Author:         florian.wang
LastModify:     2018/11
Function:       Thread safe queue unit. Easily used in producer/consumer module.
*/

#include "log.h"
#include <stdlib.h>
#include "zxThread.h"
//#include "zxLog.h"

template<typename T>
class zxQueue
{
public:
    zxQueue()
    {
        _count = 0;
        _block = true;
        _name[0] = 0;

        _read = NULL;
        _write = NULL;
        _avail = NULL;
    }

    zxQueue(const char *name)
    {
        _count = 0;
        _block = true;
        set_name(name);

        _read = NULL;
        _write = NULL;
        _avail = NULL;
    }

    ~zxQueue()
    {
        if(_count)
            LOGV("still %d items in queue %s(%x)", _count, _name, this);

        clear();
    }

    void set_name(const char *name)
    {
        if(name)
            strncpy(_name, name, 32);
        else
            sprintf(_name, "%x", (int)(long)this);
    }

    void set_block(bool block)
    {
        _block = block;
        _cond.broadcast();
    }

    bool enqueue(T data)
    {
        zxAutoLock autoLock(_mutex);
        element *item = NULL;
        if(_avail)
        {
            item = _avail;
            _avail = _avail->next;
        }
        else
        {
            item = (element *)malloc(sizeof(element));
        }

        item->next = NULL;
        item->data = data;

        if(_read == NULL)
            _read = item;

        if(_write)
            _write->next = item;

        _write = item;
        _count++;

        _cond.broadcast();
        return true;
    }

    bool dequeue(T *data)
    {
        zxAutoLock autoLock(_mutex);
        while(_read == NULL)
        {
            LOGV("queue %s(%x) is empty %d", _name, this, _count);
            _cond.wait(_mutex);

            if(!_block)
                return false;
        }

        element *item = _read;
        *data = item->data;
        _read = item->next;

        if(_read == NULL)
            _write = NULL;

        item->next = _avail;
        _avail = item;

        _count--;
        return true;
    }

    int count()
    {
        zxAutoLock autoLock(_mutex);
        return _count;
    }

    void clear()
    {
        zxAutoLock autoLock(_mutex);
        element *next;

        while(_read)
        {
            next = _read->next;
            free(_read);
            _read = next;
        }

        while(_avail)
        {
            next = _avail->next;
            free(_avail);
            _avail = next;
        }

        _count = 0;
    }

    bool get_head(T *data)
    {
        zxAutoLock autoLock(_mutex);
        while(_read == NULL)
        {
            LOGV("queue %s(%x) is empty", _name, this);
            _cond.wait(_mutex);

            if(!_block)
            {
                *data = 0;
                return false;
            }
        }

        *data = _read->data;
        return true;
    }

    bool get_tail(T *data)
    {
        zxAutoLock autoLock(_mutex);
        while(_write == NULL)
        {
            LOGV("queue %s(%x) is empty", _name, this);
            _cond.wait(_mutex);

            if(!_block)
            {
                *data = 0;
                return false;
            }
        }

        *data = _write->data;
        return true;
    }

    void dump()
    {
        LOGV("%d items in queue %s(%x)", _count, _name, this);
    }

private:
    typedef struct element
    {
        element         *next;
        T               data;
    } element;

    element         *_read;
    element         *_write;
    element         *_avail;
    int             _count;
    int             _block;
    char            _name[33];

    zxMutex         _mutex;
    zxCond          _cond;
};


#endif
