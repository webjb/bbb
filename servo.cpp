#include "servo.h"
#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

// period unit is ns
// duty unit is ns
// servo period is 20ms = 20000us = 20000000 ns
// serov duty is from 0.6ms - 2.6ms

// POS range (0-100)
// step = 2000*1000 /100 = 20*1000
#define MIN_DUTY 	(600*1000)
#define MAX_DUTY  (2600*1000)
#define DUTY_STEP 20000

#define DUTY_TO_POS(x) ( (x-MIN_DUTY)/DUTY_STEP )
#define POS_TO_DUTY(y) ( (y*DUTY_STEP + MIN_DUTY) )

char g_path[] = "/sys/devices/ocp.3";
char g_pwm_name[8][80] =
{
	"pwm_test_P9_31.12",
	"pwm_test_P9_29.13",
	"pwm_test_P8_36.14",
	"pwm_test_P8_34.15",
	"pwm_test_P8_45.16",
	"pwm_test_P8_46.17",	
};

s_servo_t::s_servo_t(string name, int id, int standby_pos, int min_pos, int max_pos)
{
	strcpy(m_name, name.c_str());
	m_id = id;	
	m_cur_duty = MIN_DUTY;

	m_standby_pos = standby_pos;
	m_min_pos = min_pos;
	m_max_pos = max_pos;

	m_is_power_on = 0;
}

s_servo_t::~s_servo_t()
{
	stop();
}

int s_servo_t::pwm_set_run(int power)
{
	char sz[280];
	sprintf(sz, "echo %d > %s/%s/run", power, g_path, g_pwm_name[m_id]);
	SYSTEM_CALL(sz);
	
	return 0;
}

int s_servo_t::pwm_set_duty(int duty)
{
	char sz[280];
	sprintf(sz, "echo %d > %s/%s/duty", m_cur_duty, g_path, g_pwm_name[m_id]);
	SYSTEM_CALL(sz);
	return 0;
}

int s_servo_t::pwm_set_period(int period)
{
	char sz[280];
	sprintf(sz, "echo %d > %s/%s/period", period, g_path, g_pwm_name[m_id]);
	SYSTEM_CALL(sz);
	return 0;
}

int s_servo_t::pwm_set_polarity(int polarity)
{
	char sz[280];
	sprintf(sz, "echo %d > %s/%s/polarity", polarity, g_path, g_pwm_name[m_id]);
	SYSTEM_CALL(sz);

	return 0;
}

int s_servo_t::power_on()
{
	if( m_id >= 8)
	{
		s_log_info("wrong id %d\n", m_id);
		return -1;
	}

	pwm_set_polarity(0);
	pwm_set_run(1);
	pwm_set_period(20000000);

	m_is_power_on = 1;
		
	return 0;	
}

int s_servo_t::power_off()
{
	if( m_id >= 8)
	{
		printf("wrong id %d\n", m_id);
		return -1;
	}
	
	pwm_set_run(0);
	m_is_power_on = 0;
	return 0;
}
int s_servo_t::standby(int speed)
{
	if( !m_is_power_on)
		power_on();
	
	m_next_duty = POS_TO_DUTY(m_standby_pos);
	s_log_info("standby id:%d duty:%d\n", m_id, m_next_duty);
	m_cur_duty = m_next_duty;
	pwm_set_duty(m_cur_duty);

	return 0;
}

int s_servo_t::is_standby_done()
{
	return 1;
}

int s_servo_t::move_to(int pos, int speed)
{
	s_log_info("s_servo_t::move_to pos:%d duty:%d speed:%d\n", pos, POS_TO_DUTY(pos), speed);
	if( m_id >= 8)
	{
		s_log_info("wrong id %d\n", m_id);
		return -1;
	}

	if( pos < m_min_pos || pos > m_max_pos)
	{
		s_log_info("move_to ERROR pos (%d) < min (%d) or > max (%d)\n", pos, m_min_pos, m_max_pos);
		return -1;
	}

	if( POS_TO_DUTY(pos) == m_cur_duty)
	{
		s_log_info("same pos do nothing\n");
		return 0;
	}
	if( speed == 0 )
	{
		s_log_info("ERROR speed is 0\n");
		return 0;
	}
	
	if( !m_is_power_on)
		power_on();
	m_speed = speed;

	m_next_duty = POS_TO_DUTY(pos);
	
	m_duty_steps = (m_next_duty - m_cur_duty)*speed/100;
	if( m_duty_steps == 0 )
	{
		m_duty_steps = 1;
	}
	
	m_state = S_STATE_RUN;
	
//	PRINT_INFO("move_to id:%d duty:%d step:%d\n", this->m_id, m_next_duty, m_duty_steps);

	return 0;
}

int s_servo_t::stop_move()
{
	m_cur_duty = m_next_duty;
	m_state = S_STATE_STOP;
	return 0;
}

int s_servo_t::get_pos()
{
	return DUTY_TO_POS(m_cur_duty);
}

int s_servo_t::start()
{
//	power_on();
	s_object_t::start();
	return 0;
}

int s_servo_t::stop()
{
	power_off();
	s_object_t::stop();
	return 0;
}

int s_servo_t::run()
{
	s_log_info("E s_servo_t::run id:%d\n", m_id);
	while ( !m_quit)
	{				
		if( m_state != S_STATE_RUN )
			goto __servo_loop;
		if( m_cur_duty == m_next_duty)
			goto __servo_loop;
		m_cur_duty += m_duty_steps;
		if( m_duty_steps > 0 )
		{
			if( m_cur_duty > m_next_duty )
				m_cur_duty = m_next_duty;
		}
		else
		{
			if( m_cur_duty < m_next_duty )
				m_cur_duty = m_next_duty;
		}

		pwm_set_duty(m_cur_duty);
__servo_loop:		
		s_sleep_ms(50);
	}
	m_quit = 2;
	
	s_log_info("X s_servo_t::run\n");	
	return 0;
}

