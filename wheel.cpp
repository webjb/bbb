// bob sang

#include "wheel.h"
#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

enum
{
	S_RUN_STATE_WH_NOTHING = 0,
	S_RUN_STATE_WH_SEARCHING,
	S_RUN_STATE_WH_SEARCHED,
	S_RUN_STATE_WH_MOVING,
	S_RUN_STATE_WH_REACHED,
	S_RUN_STATE_WH_STOP,

	S_RUN_STATE_WH_AIMING_DOOR,
	S_RUN_STATE_WH_DOOR_AIMED,

	S_RUN_STATE_WH_LOST_BALL,
};


s_wheel_t::s_wheel_t()
{
	m_j1939 = new s_j1939_t;
	m_last_pos_time = 0L;
	m_move_state = 0;
	m_eye = s_eye_t::get_inst();
	m_run_state = S_RUN_STATE_WH_NOTHING;

	m_wh_state = S_WH_STATE_NOTHING;

	m_func_state = S_FUNC_STATE_NOTHING;
}

s_wheel_t::~s_wheel_t()
{
	delete m_j1939;
}

s_wheel_t* s_wheel_t::get_inst()
{
	static s_wheel_t * g_wheel = NULL;
	if( !g_wheel )
		g_wheel = new s_wheel_t();

	return g_wheel;
}


int s_wheel_t::start()
{
	s_object_t::start();
	m_j1939->start();
	
	return 0;
}

int s_wheel_t::stop()
{
	s_log_info("wheel stop\n");
	m_j1939->set_wheels(0, 0);	
	
	s_object_t::stop();
	
	m_j1939->stop();
	
	return 0;
}

int s_wheel_t::turn_left(int speed)
{
	s_log_info("wheel:left\n");
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(speed,0);
	m_move_state = S_MOVE_STATE_LEFT;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::turn_right(int speed)
{
	s_log_info("wheel:right\n");
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(0,speed);
	m_move_state = S_MOVE_STATE_RIGHT;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}


int s_wheel_t::turn_left_2way(int speed)
{
	s_log_info("wheel:left_2way\n");
	LOCK_MUTEX(m_mutex);
//	m_j1939->set_wheels(speed,0);//-speed);
	m_j1939->set_wheels(speed-100,100-speed);
	m_move_state = S_MOVE_STATE_LEFT;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::turn_right_2way(int speed)
{
	s_log_info("wheel:right_2way\n");
	LOCK_MUTEX(m_mutex);
//	m_j1939->set_wheels(0, speed);//-speed,speed);
	m_j1939->set_wheels(100-speed,speed-100);
	m_move_state = S_MOVE_STATE_RIGHT;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}


int s_wheel_t::move_forward(int speed)
{
	s_log_info("wheel::forward\n");
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(-1*speed, -1*speed);
	m_move_state = S_MOVE_STATE_FORWARD;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}


int s_wheel_t::move_forward_turn(int left_speed, int right_speed)
{
	s_log_info("wheel::forward turn left:%d right:%d\n", left_speed, right_speed);
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(-1*left_speed, -1*right_speed);
	m_move_state = S_MOVE_STATE_FORWARD;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}


int s_wheel_t::move_backward(int speed)
{
	s_log_info("wheel::backward\n");
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(speed,speed);
	m_move_state = S_MOVE_STATE_BACKWARD;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::move_stop()
{
	LOCK_MUTEX(m_mutex);
	if( m_move_state != S_MOVE_STATE_STOP )
	{
		s_log_info("wheel:move_stop\n");
		m_j1939->set_wheels(0, 0);
		m_move_state = S_MOVE_STATE_STOP;
		m_last_cmd_time = GET_MS();
	}
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::set_position(int alpha,int z)
{
	LOCK_MUTEX(m_mutex);
	m_alpha = alpha;
	m_z = z;
	m_last_pos_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::is_position_lost()
{
	int64 t;
	t = GET_MS();
	if( t > m_last_pos_time + 3000 )
	{
		return 1;
	}
	return 0;
}

int s_wheel_t::start_search(int x)
{
	s_log_info("WHEEL_START_SEARCH alpha1:%d\n", x);
	m_run_state = S_RUN_STATE_WH_SEARCHING;	
	
	if( LINE_BALL(x) )
	{
		return 0;
	}

	if( x < 90 )
	{
		turn_right(10);
	}
	else 
	{
		turn_left(10);
	}

	m_func_state = S_FUNC_STATE_SEARCH_BALL;
	return 0;
}

int s_wheel_t::stop_search()
{
	m_run_state = S_RUN_STATE_WH_NOTHING;
	move_stop();
	return 0;
}

int s_wheel_t::start_move()
{
	s_log_info("wheel::start_move\n");
	m_run_state = S_RUN_STATE_WH_MOVING;	
	m_move_state = S_MOVE_STATE_NOTHING;
	m_func_state = S_FUNC_STATE_MOVE_TO_BALL;
	return 0;
}

int s_wheel_t::stop_move()
{
	s_log_info("wheel::stop_move\n");
	m_run_state = S_RUN_STATE_WH_NOTHING;
	move_stop();
	m_alpha = 0;
	m_z = 0;
	m_last_pos_time = 0L;
	m_move_state = S_FUNC_STATE_NOTHING;
	
	return 0;
}

int s_wheel_t::start_aim_door()
{
	s_log_info("wheel::start_aim_door\n");
	m_run_state = S_RUN_STATE_WH_AIMING_DOOR;
	m_wh_state = S_WH_STATE_NOTHING;
	m_move_state = S_MOVE_STATE_NOTHING;
	m_func_state = S_FUNC_STATE_AIM_DOOR;
	return 0;
}

int s_wheel_t::stop_aim_door()
{
	s_log_info("wheel::stop_aim_door\n");
	m_run_state = S_RUN_STATE_WH_NOTHING;
	move_stop();
	m_alpha = 0;
	m_z = 0;
	m_last_pos_time = 0L;
	m_func_state = S_FUNC_STATE_NOTHING;
	return 0;
}


int s_wheel_t::is_ball_found()
{
	return (m_run_state == S_RUN_STATE_WH_SEARCHED);
}

int s_wheel_t::is_ball_reached()
{
	return (m_run_state == S_RUN_STATE_WH_REACHED);
}

int s_wheel_t::is_door_aimed()
{
	return (m_run_state == S_RUN_STATE_WH_DOOR_AIMED);
}

int s_wheel_t::is_ball_lost()
{
	return (m_run_state == S_RUN_STATE_WH_LOST_BALL);
}

int s_wheel_t::do_search()
{
	int alpha;
	int z;
//	int64 time_ms;
	
	s_ball_postion_t	pos;
	s_eye_t * eye = s_eye_t::get_inst();
	eye->get_ball_position(&pos);
	alpha = pos.m_alpha;
	z = pos.m_distance;
	
//	time_ms = GET_MS();
	if( z < 0 )
	{	
#if 0
		move_stop();
		if( time_ms > m_last_cmd_time + 4000)
		{
			m_wh_state = S_WH_STATE_NOTHING;
			m_run_state = S_RUN_STATE_WH_LOST_BALL;
			m_move_state = S_MOVE_STATE_NOTHING;
		}
#endif	
		return 0;
	}
	m_last_z = z;
	if( m_move_state == S_MOVE_STATE_STOP )
	{
	}
	if( alpha <110 && alpha > 70)
	{
		s_log_info("%s WHEEL found ball start move alpha:%d...\n", TIME_STAMP(),  alpha);
		move_stop();
		m_run_state = S_RUN_STATE_WH_SEARCHED;
	}
	
	return 0;
}

int s_wheel_t::do_move_to_ball()
{
	int64 time_ms;
	int alpha;
	int z;
	s_ball_postion_t	pos;
	s_eye_t * eye = s_eye_t::get_inst();
	int speed;
	int left;
	int right;
	float dir;

	static int g_last_alpha = 0;
	static int g_last_z = 0;
	
	eye->get_ball_position(&pos);
	alpha = pos.m_alpha;
	z = pos.m_distance;
	
	time_ms = GET_MS();

	if( g_last_alpha != alpha && g_last_z != z)
	{
		s_log_info("%s alpha:%d z:%d\n", TIME_STAMP(), alpha, z);
		g_last_alpha = alpha;
		g_last_z = z;
	}
	if( z < 0 ) // lost ball position
	{
		move_stop();
		return 0;
	}
	if( (alpha < KICK_MAX_ANGLE && alpha > KICK_MIN_ANGLE ) &&	(z <= KICK_DISTANCE && z > 1 ) )
	{
		s_log_info("pos reached alpha:%d z:%d\n", alpha, z);
		move_stop();
		m_wh_state = S_WH_STATE_STOP;
		if( m_func_state == S_FUNC_STATE_AIM_DOOR )
		{
			s_log_info("again to aim to door\n");
			m_run_state = S_RUN_STATE_WH_AIMING_DOOR;
			m_wh_state = S_WH_STATE_NOTHING;
			m_move_state = S_MOVE_STATE_NOTHING;			
		}
		else
			m_run_state = S_RUN_STATE_WH_REACHED;
		return 0;
	}

	if( m_wh_state == S_WH_STATE_FORWARD)
	{
		if( alpha > 180 )
		{
			s_log_info("move over ball alpha:%d z:%d\n", alpha, z);
			move_stop();
			m_wh_state = S_WH_STATE_REST;
			m_last_cmd_time = time_ms;
			
			return 0;
		}
		
		if( z > 1400 )
			speed = 50;
		else if( z > 800 )
			speed = 40;
		else if( z > 500 )
			speed = 30;
		else if( z > 400 )
			speed = 20;
		else
			speed = 10;


		{
			dir = (float)(90 - alpha);
			dir = (dir/(90.0*2.0))*(float)speed;

			if( abs(dir) > 5.0)
			{
				dir = (dir>0)?5:-5;
			}

			left = speed + (int)dir;
			right = speed - (int)dir;
		}
		
		LOCK_MUTEX(m_mutex);
		m_j1939->set_wheels(-1*left, -1*right);
		m_move_state = S_MOVE_STATE_FORWARD;
		m_last_cmd_time = time_ms;
		UNLOCK_MUTEX(m_mutex);
	}

	if( m_wh_state == S_WH_STATE_REST)
	{
		if( time_ms > m_last_cmd_time + 1000)
		{
			m_wh_state = S_WH_STATE_NOTHING;
			s_log_info("reset done, nothing\n");
			return 0;
		}
	}

	if( m_wh_state == S_WH_STATE_NOTHING)
	{
		if( alpha > 180 )
		{
			move_backward(10);
			m_wh_state = S_WH_STATE_BACKWARD;
		}
		else 
		{
			m_wh_state = S_WH_STATE_FORWARD;
			return 0;
		}
	}

	if( m_wh_state == S_WH_STATE_BACKWARD )
	{
		if( ( (alpha < 180) && (z > KICK_DISTANCE) ) || (time_ms > m_last_cmd_time + 5000) )
		{
			move_stop();
			m_wh_state = S_WH_STATE_REST;
			m_last_cmd_time = time_ms;
			s_log_info("reset start\n");
			return 0;
		}
	}

	return 0;
}

int s_wheel_t::do_aim_to_door()
{
	int64 time_ms;
	s_door_position_t dpos;
	s_eye_t * eye = s_eye_t::get_inst();
	int alpha;
	int z;
	s_ball_postion_t	pos;

	static int g_door_x = 0;
	static int g_door_y = 0;
	
	eye->get_ball_position(&pos);
	alpha = pos.m_alpha;
	z = pos.m_distance;

	if( z < 0 )
	{
//		PRINT_INFO("aim to door lost ball\n");
//		return 0;
	}
	time_ms = GET_MS();

	eye->get_door_position(&dpos);

	if( dpos.m_width > 0 )
	{
		if( g_door_x != dpos.m_x && g_door_y != dpos.m_y)
		{
			PRINT_INFO("door_pos:%d %d\n", dpos.m_x, dpos.m_y);
			g_door_x = dpos.m_x;
			g_door_y = dpos.m_y;
		}
	
		if( (dpos.m_x > -50 && dpos.m_x < 50) 
			&& 	( (alpha < KICK_MAX_ANGLE && alpha > KICK_MIN_ANGLE ) &&	(z <= KICK_DISTANCE && z > 1 )))
		{
	
			move_stop();
			s_log_info("door aimed\n");
			m_run_state = S_RUN_STATE_WH_DOOR_AIMED;
			return 0;
		}
	}
	if( m_wh_state == S_WH_STATE_NOTHING)
	{		
		if( dpos.m_width <= 0)
		{
			s_log_info("\naim to door lost door\n");
			move_stop();
			m_eye->search(10);
			m_wh_state = S_WH_STATE_AIM_SEARCH_DOOR;
			return 0;
		}
		s_log_info("turn door x:%d y:%d\n", dpos.m_x, dpos.m_y);
		if( dpos.m_x <= -50 )
		{
			if( m_wh_state != S_WH_STATE_AIM_LEFT)
			{
				turn_left_2way(10);
				m_wh_state = S_WH_STATE_AIM_LEFT;
			}				
		}		
		else if( dpos.m_x >= 50 )
		{
			if( m_wh_state != S_WH_STATE_AIM_RIGHT)
			{
				turn_right_2way(10);
				m_wh_state = S_WH_STATE_AIM_RIGHT;
			}				
		}
		else
		{
			m_wh_state = S_WH_STATE_NOTHING;
			m_run_state = S_RUN_STATE_WH_MOVING;	
			m_move_state = S_MOVE_STATE_NOTHING;
		}
		return 0;
	}

	if( m_wh_state == S_WH_STATE_AIM_SEARCH_DOOR )
	{
		if( dpos.m_width > 0 )
		{
			m_eye->stop_search();			
			m_wh_state = S_WH_STATE_NOTHING;
			return 0;
		}
	}

	if( (m_wh_state == S_WH_STATE_AIM_LEFT) || (m_wh_state == S_WH_STATE_AIM_RIGHT) )
	{
		if( time_ms > m_last_cmd_time + 2000 )
		{
			move_stop();
			m_wh_state = S_WH_STATE_AIM_REST_1;
		}
	}
	if( m_wh_state == S_WH_STATE_AIM_REST_1)
	{
		if( time_ms > m_last_cmd_time + 1000 )
		{
			move_backward(15);
			m_wh_state = S_WH_STATE_AIM_REST_2;
		}		
	}
	if( m_wh_state == S_WH_STATE_AIM_REST_2)
	{
		if( time_ms > m_last_cmd_time + 2000 )
		{
			move_stop();
			s_log_info("drift done\n");
			if( z < 0 )
			{
				m_wh_state = S_WH_STATE_NOTHING;
				m_run_state = S_RUN_STATE_WH_LOST_BALL;
				m_move_state = S_MOVE_STATE_NOTHING;
			}
			else
			{
				m_wh_state = S_WH_STATE_NOTHING;
				m_run_state = S_RUN_STATE_WH_MOVING;	
				m_move_state = S_MOVE_STATE_NOTHING;
			}			
			m_eye->go_pos_1(10);
		}
	}
	return 0;
}

int s_wheel_t::run()
{
	s_log_info("E s_wheel_t::run()\n");
	
	m_last_cmd_time = s_timer_t::get_inst()->get_ms();
	while(!m_quit)
	{		
		if( m_run_state == S_RUN_STATE_WH_SEARCHING)
		{
			do_search();
		}		
		else if ( m_run_state == S_RUN_STATE_WH_MOVING )
		{
			do_move_to_ball();
		}
		else if( m_run_state == S_RUN_STATE_WH_REACHED )
		{		
		}
		else if( m_run_state == S_RUN_STATE_WH_AIMING_DOOR)
		{
			do_aim_to_door();
		}
		else if( m_run_state == S_RUN_STATE_WH_DOOR_AIMED )
		{
		}
		s_sleep_ms(10);
	}
	m_quit = 1;
	
	s_log_info("X s_wheel_t::run()\n");
	return 0;
}

