// bob sang

#include "arm.h"
#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

s_arm_t::s_arm_t(int id)
{
	int i;
	for( i=0; i<MAX_SERVOS; i++)
	{
		m_servo[i] = NULL;
	}
	m_servo[0] = new s_servo_t("shoulder", 0, 45, 10, 90);
	m_servo[1] = new s_servo_t("arm1", 1, 50, 40, 90);
	m_servo[2] = new s_servo_t("arm2", 2, 90, 40, 95);
//	m_servo[3] = new s_servo_t("arm3", 3);

	m_id = id;
	m_cmd_list.clear();

	m_is_power_on = 0;
}

s_arm_t::~s_arm_t()
{
	int i;
	for(i=0; i<MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			delete m_servo[i];
	}
}

s_arm_t* s_arm_t::get_inst()
{
	static s_arm_t * g_arm = NULL;
	if( !g_arm )
		g_arm = new s_arm_t(0);

	return g_arm;
}


int s_arm_t::add_power_off_cmd(int servo_id, int timer_ms)
{
	s_arm_command_t cmd;
	int64 time_ms;

	time_ms = s_timer_t::get_inst()->get_ms();
	
	s_memset(&cmd, 0, sizeof(s_arm_command_t));
	cmd.m_start_time_ms = time_ms + timer_ms;
	cmd.m_servo_id = servo_id;
	cmd.m_command = S_CMD_POWER_OFF;
	
	m_cmd_list.push_back(cmd);

	return 0;

}

int s_arm_t::add_command(int servo_id, int timer_ms, int pos, int speed)
{
	s_arm_command_t cmd;
	int64 time_ms;

	time_ms = s_timer_t::get_inst()->get_ms();
//	PRINT_INFO("%s time:%lld pos:%d speed:%d\n", TIME_STAMP(), time_ms, pos, speed);
	
	s_memset(&cmd, 0, sizeof(s_arm_command_t));
	cmd.m_start_time_ms = time_ms + timer_ms;
	cmd.m_servo_id = servo_id;
	cmd.m_command = S_CMD_MOVE_TO;
	cmd.m_speed = speed;
	cmd.m_pos = pos;
//	cmd.m_duration = 1000;
//	cmd.m_event.m_type = S_EVENT_NOTHING;
	m_cmd_list.push_back(cmd);

	return 0;
}

int s_arm_t::standby(int speed)
{
	s_log_info("STANDBY %d\n", speed);
#if 0	
	m_servo[0]->standby(speed);
	usleep(1000*2000);
	m_servo[1]->standby(speed);
	usleep(1000*2000);
	m_servo[2]->standby(speed);
	usleep(1000*2000);
#endif	
	return 0;
}

int s_arm_t::go_pos_front(int speed)
{
	s_log_info("go_pos_front %d\n", speed);
	add_command(0, 0000, 45, 100);
	add_command(1, 1000, 50, 100);
	add_command(2, 1000, 90, 100);
	return 0;
}

int s_arm_t::kick(int speed)
{
	s_log_info("KICK %d\n", speed);
	add_command(1, 0000, 50, 100);
	add_command(2, 0000, 90, 100);
	add_command(0, 1000, 45, 100);

	add_command(0, 3000, 45, speed);
//	add_command(2, 4100, 80, speed);
	add_command(1, 3100, 85, speed);
	
	add_command(2, 5000, 40, 100);

	add_command(2, 7000, 90, 100);
	add_command(1, 7000, 50, 100);
	add_command(0, 8000, 45, 100);
	
	return 0;
}

int s_arm_t::right_low(int speed)
{
	s_log_info("RIGHT_LOW %d\n", speed);
	add_command(0, 0000, 85, 100);//speed);
	add_command(2, 0500, 40, 100);//speed);
	add_command(1, 1000, 90, 100);//speed);
	add_power_off_cmd(0, 3000);
	add_power_off_cmd(2, 3500);
	add_power_off_cmd(1, 4000);
	return 0;
}


int s_arm_t::up(int speed)
{
	s_log_info("up %d\n", speed);
	add_command(0, 1000, 50, speed);
	add_command(1, 4000, 20, speed);
	
	return 0;
}

int s_arm_t::down(int speed)
{
	s_log_info("down %d\n", speed);
	add_command(0, 1000, 0, speed);
	add_command(1, 3000, 100, speed);
	return 0;
}

int s_arm_t::left(int speed)
{
	s_log_info("left %d\n", speed);
	return 0;
}

int s_arm_t::right(int speed)
{
	s_log_info("right %d\n", speed);
	return 0;
}

int s_arm_t::poweroff()
{
	s_log_info("poweroff\n");
	m_servo[0]->power_off();
	m_servo[1]->power_off();
	m_servo[2]->power_off();
	
	return 0;
}

int s_arm_t::is_command_all_done()
{
	if( m_cmd_list.empty() )
		return 1;
	else
		return 0;
}


int s_arm_t::start()
{
	int i;
	s_object_t::start();
	for( i=0; i<MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			m_servo[i]->start();
	}
	
	return 0;
}

int s_arm_t::stop()
{
	int i;
	for( i=0; i<MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			m_servo[i]->stop();
	}
	s_object_t::stop();
	m_cmd_list.clear();
	
	return 0;
}

bool s_arm_t::is_event_reached(s_arm_command_t cmd)
{
	int64 time_ms;

	time_ms = s_timer_t::get_inst()->get_ms();

//	PRINT_INFO("is %lld %lld\n", time_ms, cmd.m_start_time_ms);
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
//		int cur_time;
//		cur_time = get_current_time();
//		if( cur_time > cmd.m_event.m_timer )
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

int s_arm_t::do_command(s_arm_command_t cmd)
{
	s_log_info("%s do_command\n", TIME_STAMP());
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


int s_arm_t::run()
{
	s_log_info("E s_arm_t::run()\n");
	while(!m_quit)
	{
		list<s_arm_command_t>::iterator it;
		if( m_cmd_list.empty()  )
		{				
			goto __arm_loop;
		}

		for(it = m_cmd_list.begin(); it != m_cmd_list.end(); it ++)
		{
			if( is_event_reached(*it) ) 
			{
				do_command(*it);
				m_cmd_list.erase(it);
				break;
			}
		}
		s_sleep_ms(20);
		continue;
__arm_loop:
		s_sleep_ms(100);
	}
	m_quit = 1;
	s_log_info("X s_arm_t::run()\n");
	return 0;
}
