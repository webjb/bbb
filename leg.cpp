// bob sang

#include "leg.h"

s_leg_t::s_leg_t()
{
	m_j1939 = new s_j1939_t();
}

s_leg_t::~s_leg_t()
{
	delete m_j1939;
}


int s_leg_t::start()
{
	s_object_t::start();
	m_j1939->start();
	
	return 0;
}

int s_leg_t::stop()
{
	s_object_t::stop();
	m_j1939->stop();
	
	return 0;
}

int s_leg_t::run()
{
	PRINT_INFO("E s_leg_t::run()\n");
	while(!m_quit)
	{
	}
	m_quit = 1;
	PRINT_INFO("X s_leg_t::run()\n");
	return 0;
}

int s_leg_t::set_speed(int speed)
{
	m_speed = speed;
	return 0;
}

int s_leg_t::left(int speed)
{
	if( speed != 0 )
		set_speed(speed);

	m_j1939->set_wheels(m_speed, 0);
	return 0;
}

int s_leg_t::right(int speed)
{
	if( speed != 0 )
		set_speed(speed);
	
	m_j1939->set_wheels(0, m_speed);

	return 0;
}

int s_leg_t::forward(int speed)
{
	if( speed != 0 )
		set_speed(speed);
	
	m_j1939->set_wheels(m_speed, m_speed);

	return 0;
}

int s_leg_t::backward(int speed)
{
	if( speed != 0 )
		set_speed(speed);
	
	m_j1939->set_wheels(m_speed, 0);

	return 0;
}

int s_leg_t::back_left(int speed )
{
	if( speed != 0 )
		set_speed(speed);

	m_j1939->set_wheels(m_speed, 0);
	return 0;
}

int s_leg_t::back_right(int speed)
{
	if( speed != 0 )
		set_speed(speed);
	
	m_j1939->set_wheels(0, m_speed);

	return 0;
}

