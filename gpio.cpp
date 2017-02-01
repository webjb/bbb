#include "gpio.h"
#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

s_gpio_t::s_gpio_t(int io_id, int bit)
{
	m_id = io_id;
	m_bit = bit;
	m_status = -1;
}

s_gpio_t::~s_gpio_t()
{
}

int s_gpio_t::power_on()
{
	char sz[280];
	int id;
	s_log_info("gpio power on\n");
	id = m_id*32 + m_bit;
	sprintf(sz, "echo %d > /sys/class/gpio/export", id);
	SYSTEM_CALL(sz);
	sprintf(sz, "echo out > /sys/class/gpio/gpio%d/direction", id);
	SYSTEM_CALL(sz);

	printf("gpio power on %s\n", sz);
	return 0;
}

int s_gpio_t::power_off()
{
	char sz[280];
	int id;
	id = m_id*32 + m_bit;
	sprintf(sz, "echo %d > /sys/class/gpio/unexport", id);
	SYSTEM_CALL(sz);

	s_log_info("gpio power off %s\n", sz);
	return 0;
}

int s_gpio_t::set_high()
{
	if( m_status == 1 )
		return 0;
	int id;
	m_status = 1;

	id = m_id * 32 + m_bit;
	
	char sz[280];
	sprintf(sz, "echo 0 > /sys/class/gpio/gpio%d/value", id);
	SYSTEM_CALL(sz);

	s_log_info("gpio high: %s\n", sz);
	return 0;
}

int s_gpio_t::set_low()
{
	if( m_status == 0 )
		return 0;
	int id;
	m_status = 0;

	id = m_id * 32 + m_bit;
	
	char sz[280];
	sprintf(sz, "echo 1 > /sys/class/gpio/gpio%d/value", id);
	SYSTEM_CALL(sz);

	s_log_info("gpio low: %s\n", sz);
	return 0;
	
}

int s_gpio_t::start()
{
	s_log_info("gpio start id:%d bit:%d\n", m_id, m_bit);
	s_object_t::start();
	power_on();
	return 0;
}

int s_gpio_t::stop()
{
	s_log_info("gpio stop id:%d bit:%d\n", m_id, m_bit);
//	power_off();
	s_object_t::stop();
	return 0;
}

int s_gpio_t::run()
{
	s_log_info("E s_gpio_t::run id:%d\n", m_id);
	while ( !m_quit)
	{				
		s_sleep_ms(50);
	}
	m_quit = 2;
	
	s_log_info("X s_gpio_t::run\n");	
	return 0;
}

