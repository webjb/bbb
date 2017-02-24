#ifndef __S_DEF_H__
#define __S_DEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <pthread.h>
#include <semaphore.h>
#include <dirent.h>
#include <ctype.h>

#include <list>

#include <stddef.h>
#include <signal.h>

#include <execinfo.h>
#include <unwind.h>

#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>

//nclude <linux/can.h>
//nclude <linux/can/raw.h>

using namespace std;


#define thread_t    pthread_t

#define mutex_t     pthread_mutex_t
#define signal_t    pthread_cond_t

#define CREATE_THREAD(thread, func, stack_size, para, detached)                                 \
                                    {                                                           \
                                        pthread_attr_t thread_attr;                             \
                                        pthread_attr_init(&thread_attr);                        \
                                        pthread_attr_setstacksize(&thread_attr, (stack_size));  \
                                        if (detached)                                           \
                                        {                                                       \
                                            pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED); \
                                        }                                                       \
                                        pthread_create(&(thread), &thread_attr, (func), (para));\
                                        pthread_attr_destroy(&thread_attr);                     \
                                    }
#define EXIT_THREAD(err)            pthread_exit(err)


#define CREATE_MUTEX(mutex)         pthread_mutex_init(&(mutex), NULL)  //((mutex) = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER)
#define DESTROY_MUTEX(mutex)        pthread_mutex_destroy(&(mutex))
#define TRY_LOCK_MUTEX(mutex)       pthread_mutex_trylock(&(mutex))
#define LOCK_MUTEX(mutex)           pthread_mutex_lock(&(mutex))
#define UNLOCK_MUTEX(mutex)         pthread_mutex_unlock(&(mutex))

#define CREATE_SIGNAL(signal, max)  pthread_cond_init(&(signal), NULL)  //((signal) = (pthread_cond_t)PTHREAD_COND_INITIALIZER)
#define DESTROY_SIGNAL(signal)      pthread_cond_destroy(&(signal))

#define WAIT_SIGNAL(signal, mutex)  pthread_cond_wait(&(signal), &(mutex))

#define WAIT_SIGNAL_TIMEOUT(signal, mutex, timeout) pthread_cond_timedwait(&(signal), &(mutex), &(timeout))
#define SIGNAL(signal)              pthread_cond_signal(&(signal))

#define MAX_NAME_LENGTH		255

#ifndef int64 
#define int64 long long
#endif


enum {
    S_ERROR_NONE = 0,
    S_ERROR_CAN_BUS_NOT_EXIST = -1000,
    S_ERROR_CAN_BUS_NO_DATA  = -1001,
    
};

#endif

