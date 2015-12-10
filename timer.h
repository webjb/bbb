// bob sang

#ifndef __S_TIMER_H__
#define __S_TIMER_H__

#include "common.h"

class s_timer_t
{
public:
    s_timer_t();
    ~s_timer_t();
public:
    static s_timer_t * get_inst();
    int64 get_ms();
    int64 get_us();
    string get_str();

    int64 m_sec_0;
};

#endif
