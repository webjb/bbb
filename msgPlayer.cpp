//bob sang robot

#include "msgPlayer.h"

#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

#define MSG_MAX_SERVOS 3

s_massage_player_t::s_massage_player_t()
{
	int i;
	for( i=0; i<MAX_SERVOS; i++)
	{
		m_servo[i] = NULL;
	}
	m_servo[0] = new s_servo_t("shoulder", 0, 45, 10, 90);
	m_servo[1] = new s_servo_t("arm1", 1, 50, 10, 90);
	m_servo[2] = new s_servo_t("arm2", 2, 50, 10, 95);
//	m_servo[3] = new s_servo_t("arm3", 3);

	m_gpio[0] = new s_gpio_t(2,13);
	m_gpio[1] = new s_gpio_t(2,12);
	
	m_cmd_list.clear();

}

s_massage_player_t::~s_massage_player_t()
{
	int i;
	for(i=0; i<MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			delete m_servo[i];
	}

	delete m_gpio[0];
	delete m_gpio[1];
}

s_massage_player_t* s_massage_player_t::get_inst()
{
	static s_massage_player_t * g_massage_player = NULL;
	if( !g_massage_player )
		g_massage_player = new s_massage_player_t();

	return g_massage_player;
}

int s_massage_player_t::add_command(int servo_id, int timer_ms, int pos, int speed)
{
	s_msg_command_t cmd;
	int64 time_ms;

	time_ms = s_timer_t::get_inst()->get_ms();
	
	s_memset(&cmd, 0, sizeof(s_msg_command_t));
	cmd.m_start_time_ms = time_ms + timer_ms;
	cmd.m_servo_id = servo_id;
	cmd.m_command = S_CMD_MOVE_TO;
	cmd.m_speed = speed;
	cmd.m_pos = pos;
	m_cmd_list.push_back(cmd);

	return 0;
}


bool s_massage_player_t::is_event_reached(s_msg_command_t cmd)
{
	int64 time_ms;

	time_ms = s_timer_t::get_inst()->get_ms();

	if( cmd.m_start_time_ms == -1 )
		return true;

	if( time_ms >= cmd.m_start_time_ms )
		return true;
	else
		return false;

	if( cmd.m_event.m_type == S_EVENT_NOTHING)
		return true;

	if(cmd.m_event.m_type == S_EVENT_TIMER )
	{
		return true;		
	}

	if( cmd.m_event.m_type == S_EVENT_SERVO_POS_REACHED )
	{
		int id;
		int pos;
		id = cmd.m_event.m_servo_id;
		pos = m_servo[id]->get_pos();

		if( cmd.m_event.m_dir == 1 )
		{
			if( pos >= cmd.m_event.m_pos )
				return true;
		}
		else 
		{
			if( pos <= cmd.m_event.m_pos )
				return true;
		}
	}
	return false;
}

int s_massage_player_t::do_command(s_msg_command_t cmd)
{
	PRINT_INFO("%s do_command\n", TIME_STAMP());
	switch(cmd.m_command)
	{
		case S_CMD_MOVE_TO:		
			m_servo[cmd.m_servo_id]->move_to(cmd.m_pos, cmd.m_speed);
			break;
		case S_CMD_POWER_OFF:
			m_servo[cmd.m_servo_id]->power_off();
			break;
	}
	return 0;
}

int s_massage_player_t::play()
{
	int time;
	int i;
	int pos;
	add_command(0, 0000, 45, 100);
	add_command(1, 1000, 70, 100);
	add_command(2, 3000, 70, 100);
	time = 3000;
	for( i=0; i<400; i++)
	{
		if( (i%2) == 0 )
		{
			pos = 70;
			time += 200;
		}
		else
		{
			pos = 80;
			time += 500;
		}
		add_command(2, time, pos, 100);
	}
		

	return 0;
}

int s_massage_player_t::start()
{
#if 0
	int i;
	for( i=0; i<MSG_MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			m_servo[i]->start();
	}

	play();
#endif	

	m_gpio[0]->start();
	m_gpio[0]->set_low();
	
	m_gpio[1]->start();
	m_gpio[1]->set_low();
	
	s_object_t::start();
	return 0;
}

int s_massage_player_t::stop()
{
#if 0
	int i;
	PRINT_INFO("massage stop\n");
	m_servo[0]->power_off();
	m_servo[1]->power_off();
	m_servo[2]->power_off();
	
	for( i=0; i<EYE_MAX_SERVOS; i++)
	{
		if( m_servo[i] )
		{
			PRINT_INFO("msg stop %d\n", i);
			m_servo[i]->stop();
		}
	}
#endif

	s_object_t::stop();
	m_gpio[0]->stop();	
	m_gpio[1]->stop();

	s_log_info("msgPlayer stop done\n");
	
	LOCK_MUTEX(m_mutex);
	m_cmd_list.clear();
	UNLOCK_MUTEX(m_mutex);
	
	return 0;
}

#if 0
int s_massage_player_t::run()
{
	while(!m_quit)
	{
		list<s_msg_command_t>::iterator it;
		if( m_cmd_list.empty()	)
		{				
			goto __msg_loop;
		}
		LOCK_MUTEX(m_mutex);
		for(it = m_cmd_list.begin(); it != m_cmd_list.end(); it ++)
		{
			if( is_event_reached(*it) ) 
			{
				do_command(*it);
				m_cmd_list.erase(it);
				break;
			}
		}
		UNLOCK_MUTEX(m_mutex);
		usleep(1000*20);
		continue;
__msg_loop:
		usleep(1000*100);
	}
	m_quit = 1;
	return 0;
}
#endif


#define SLEEP_ON	10

#define SLEEP_OFF	10

int s_massage_player_t::run()
{
	int count;
	int dir = 0;
	while(!m_quit)
	{	
//		m_gpio[1]->set_low();
		if (dir == 0 )
		{
			m_gpio[0]->set_low();
			dir = 1;
			
			s_log_info("high\n");
			m_gpio[1]->set_high();			
			count = 0;
			while(!m_quit)
			{
				s_sleep_ms(10);
				if( count ++ > SLEEP_ON )
					break;
			}
			if( m_quit )
				break;
			
			s_log_info("low\n");
			count = 0;
			m_gpio[1]->set_low();
			while(!m_quit)
			{
				s_sleep_ms(10);
				if( count ++ > SLEEP_OFF )
					break;
			}
		}
		else
		{
			m_gpio[0]->set_high();
			dir = 0;
			
			s_log_info("low\n");
			count = 0;
			m_gpio[1]->set_low();
			while(!m_quit)
			{
				s_sleep_ms(10);
				if( count ++ > SLEEP_ON )
					break;
			}

			s_log_info("high\n");
			m_gpio[1]->set_high();			
			count = 0;
			while(!m_quit)
			{
				s_sleep_ms(10);
				if( count ++ > SLEEP_OFF )
					break;
			}
			if( m_quit )
				break;
				
		}
		
	}
	s_log_info("\n\n------------------quit run-------\n");
	m_quit = 1;
	m_gpio[0]->set_low();
	m_gpio[1]->set_low();
	
	return 0;
}

