//
#include <sys/time.h>

#include "timer.h"

static s_timer_t * g_inst = NULL;
s_timer_t::s_timer_t()
{
    struct timeval current;
    gettimeofday(&current, NULL);
	m_sec_0 = current.tv_sec;
}

s_timer_t::~s_timer_t()
{
}

s_timer_t * s_timer_t::get_inst()
{
	if( !g_inst )
	{
		g_inst = new s_timer_t();
	}
	return g_inst;
}

int64 s_timer_t::get_ms()
{
    struct timeval current;
    gettimeofday(&current, NULL);
    return (int64)(current.tv_sec - m_sec_0) * 1000 + (int64)current.tv_usec / 1000;
}

int64 s_timer_t::get_us()
{
	int64 ret;
    struct timeval current;
    gettimeofday(&current, NULL);
    ret  = (int64) (current.tv_sec - m_sec_0)* 1000 *1000 + (int64)current.tv_usec;
	return ret;
}

string s_timer_t::get_str()
{
	char sz[80];
	string str;
    struct timeval current;
    gettimeofday(&current, NULL);
	
	sprintf(sz, "%d.%d", (int)(current.tv_sec - m_sec_0), (int)current.tv_usec/1000);
	str =sz;
	return str;
}

