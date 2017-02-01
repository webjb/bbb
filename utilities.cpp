// bob sang
#include "utilities.h"

namespace s_utilities
{
	s_utilities_t::s_utilities_t()
	{
	}

	s_utilities_t::~s_utilities_t()
	{
	}

	void * s_memset(void * addr, int v, size_t size)
	{
		return memset(addr, v, size);
	}

	void s_sleep_ms(int msec)
	{
		usleep(1000*msec);
	}
	
	void s_sleep_us(int usec)
	{
		usleep(usec);
	}
}
