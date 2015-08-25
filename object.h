// bob sang robot

#ifndef __S_OBJECT_H__
#define __S_OBJECT_H__

#include "common.h"

class s_object_t
{
public:
    s_object_t();
    virtual ~s_object_t();
private:
    static void * thread_entry(void *);
	
public:
    virtual int start() = 0;
    virtual int stop() = 0;        
	virtual int run() = 0;
public:        
    int is_started();        
    mutex_t m_mutex;
        
protected:
    int  m_quit;    
private:
   
    thread_t m_thread;
    signal_t m_signal;  
};
#endif

