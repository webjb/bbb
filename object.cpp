// bob sang
#include "object.h"

s_object_t::s_object_t()
{
//	PRINT_INFO("xc_ao_t::xc_ao_t\n");
    m_quit = 1;	
    CREATE_MUTEX(m_mutex);
    CREATE_SIGNAL(m_signal, 0);	
}

s_object_t::~s_object_t()
{
//	PRINT_INFO("xc_ao_t::~xc_ao_t\n");
	s_object_t::stop();
    DESTROY_MUTEX(m_mutex);
	DESTROY_SIGNAL(m_signal);
}


int s_object_t::start()
{
//	PRINT_INFO("xc_ao_t::start\n");

	if( m_quit == 0 )
	{
		PRINT_INFO("started\n");
		return 0;
	}
	m_quit = 0;
    CREATE_THREAD(m_thread, thread_entry, 0x8000, this, 1);         // Reserve 32KB stack
    return 0;
}

int s_object_t::stop()
{
	int ret = 0;

	if( m_quit )
		return 0;
	
//	PRINT_INFO("E xc_ao_t::stop\n");
    m_quit = 1;	
#if 1
	timespec abstime;
	//	struct timeval current;
	clock_gettime(CLOCK_REALTIME, &abstime);
//	abstime.tv_sec += 1;	// timed out 6 seconds
	abstime.tv_nsec += 1000*1000*10;
	
	
	LOCK_MUTEX(m_mutex);
	ret = WAIT_SIGNAL_TIMEOUT(m_signal, m_mutex, abstime);
	UNLOCK_MUTEX(m_mutex);
#endif		
//	PRINT_INFO("X xc_ao_t::stop\n");
    return ret;
}

void * s_object_t::thread_entry(void * pval)
{
   	s_object_t *preg = (s_object_t*)pval;
    preg->run();
	SIGNAL(preg->m_signal);

	return NULL;
}

int s_object_t::is_started()
{
	return (m_quit == 0);
}

