//bob sang
#ifndef __S_UTILITIES_H__
#define __S_UTILITIES_H__

#include "common.h"

namespace s_utilities
{
    void * s_memset(void * addr, int v, size_t size);
    void s_sleep_ms(int msec);
    void s_sleep_us(int usec);
    
    class s_utilities_t
    {
        s_utilities_t();
        ~s_utilities_t();
        public:        
    };
}

#endif
