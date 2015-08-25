// bob sang

#include "wheel.h"
enum
{
	S_RUN_STATE_WH_NOTHING = 0,
	S_RUN_STATE_WH_SEARCHING,
	S_RUN_STATE_WH_SEARCHED,
	S_RUN_STATE_WH_MOVING,
	S_RUN_STATE_WH_REACHED,
	S_RUN_STATE_WH_STOP,
};


s_wheel_t::s_wheel_t()
{
	m_j1939 = new s_j1939_t;
	m_last_pos_time = 0L;
	m_move_state = 0;
	m_eye = s_eye_t::get_inst();
	m_run_state = S_RUN_STATE_WH_NOTHING;
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
	s_object_t::stop();
	m_j1939->stop();
	
	return 0;
}

int s_wheel_t::turn_left(int speed)
{
	PRINT_INFO("wheel:left\n");
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(speed,0);
	m_move_state = S_MOVE_STATE_LEFT;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::turn_right(int speed)
{
	PRINT_INFO("wheel:right\n");
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(0,speed);
	m_move_state = S_MOVE_STATE_RIGHT;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::move_forward(int speed)
{
	PRINT_INFO("wheel::forward\n");
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(-1*speed, -1*speed);
	m_move_state = S_MOVE_STATE_FORWARD;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::move_backward(int speed)
{
	PRINT_INFO("wheel::backward\n");
	LOCK_MUTEX(m_mutex);
	m_j1939->set_wheels(speed,speed);
	m_move_state = S_MOVE_STATE_BACKWARD;
	m_last_cmd_time = GET_MS();
	UNLOCK_MUTEX(m_mutex);
	return 0;
}

int s_wheel_t::move_stop()
{
	PRINT_INFO("wheel:move_stop\n");
	LOCK_MUTEX(m_mutex);
	if( m_move_state != S_MOVE_STATE_STOP )
	{
		m_j1939->set_wheels(0, 0);
		m_last_cmd_time = GET_MS();
		m_move_state = S_MOVE_STATE_STOP;
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

int s_wheel_t::start_search()
{
	int alpha;
	s_ball_postion_t pos;
	
	m_eye->get_ball_position(&pos);
	alpha = pos.m_alpha;

	PRINT_INFO("WHEEL_START_SEARCH alpha1:%d\n", alpha);
	m_run_state = S_RUN_STATE_WH_SEARCHING;	
	
	if( LINE_BALL(alpha) )
	{
		return 0;
	}
	
	if( alpha < 90 )
	{
		turn_right(10);
	}
	else 
	{
		turn_left(10);
	}
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
	PRINT_INFO("wheel::start_move\n");
	m_run_state = S_RUN_STATE_WH_MOVING;
	
	m_move_state = S_MOVE_STATE_NOTHING;
	return 0;
}

int s_wheel_t::stop_move()
{
	PRINT_INFO("wheel::stop_move\n");
	m_run_state = S_RUN_STATE_WH_NOTHING;
	move_stop();
	m_alpha = 0;
	m_z = 0;
	m_last_pos_time = 0L;
	
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

int s_wheel_t::run()
{
	PRINT_INFO("E s_wheel_t::run()\n");
	int alpha;
	int z;
	int64 time_ms;
	
	m_last_cmd_time = s_timer_t::get_inst()->get_ms();
	while(!m_quit)
	{
#if 0	
		LOCK_MUTEX(m_mutex);
		alpha = m_alpha;
		z = m_z;
		UNLOCK_MUTEX(m_mutex);
#endif
		s_ball_postion_t	pos;
		s_eye_t * eye = s_eye_t::get_inst();
		eye->get_ball_position(&pos);
		alpha = pos.m_alpha;
		z = pos.m_distance;
		
		time_ms = GET_MS();

		if( m_run_state == S_RUN_STATE_WH_SEARCHING)
		{
			if( (z > 0) && (alpha > 0) )
			{
				PRINT_INFO("WHEEL found ball start move...\n");
				move_stop();
				m_run_state = S_RUN_STATE_WH_SEARCHED;
			}
		}		
		else if ( m_run_state == S_RUN_STATE_WH_MOVING )
		{
			if( (alpha < KICK_MAX_ANGLE && alpha > KICK_MIN_ANGLE ) &&	(z <= KICK_DISTANCE && z > 1 ) )
			{
				move_stop();
				m_move_state = S_MOVE_STATE_STOP;
				m_run_state = S_RUN_STATE_WH_REACHED;
				goto _end;
			}
			switch( m_move_state )
			{
				case S_MOVE_STATE_STOP:
					break;
				case S_MOVE_STATE_FORWARD:
				case S_MOVE_STATE_BACKWARD:
					if( time_ms > m_last_cmd_time + 1000 ) // 3 seconds
					{
						PRINT_INFO("run state from %d to rest\n", m_run_state);
						move_stop();
						m_move_state = S_MOVE_STATE_REST;
						m_last_cmd_time = time_ms;
					}
					break;
					
				case S_MOVE_STATE_LEFT:
				case S_MOVE_STATE_RIGHT:
					if( time_ms > m_last_cmd_time + 1000 ) // 
					{
						PRINT_INFO("run state from %d to rest\n", m_run_state);
						move_stop();
						m_move_state = S_MOVE_STATE_REST;
						m_last_cmd_time = time_ms;
					}
					break;
				case S_MOVE_STATE_REST:
					if( time_ms > m_last_cmd_time + 1600 ) // 
					{
						PRINT_INFO("run state from %d to nothing\n", m_run_state);
						m_move_state = S_MOVE_STATE_NOTHING;
						m_last_cmd_time = time_ms;
					}
					break;
				default:
					break;
			}
		
			if( m_move_state != S_MOVE_STATE_NOTHING)
				goto _end;

//			if( is_position_lost() )
//				goto _end;
			if( z <= 0 )
			{
				PRINT_INFO("lost position...\n");
				goto _end;
			}
			PRINT_INFO("z=%d alpha:%d\n", z, alpha);
			
			if( z > KICK_DISTANCE )
			{
				if( alpha >= 80 && alpha <= 100)
				{
					// move forward
					if( z < 400 )
						move_forward(10);
					else if( z < 800 )
						move_forward(20);
					else if( z < 1400 )
						move_forward(30);
					else 
						move_forward(40);
				}
				else if( alpha < 80  )
				{
					// turn right
					turn_right(10);
				}
				else if( (alpha > 100) && (alpha < 180) )
				{
					// turn left
					turn_left(10);
				}
				else 
				{
					move_backward(10);
				}
			}
			else
			{
				move_backward(10);
			}
		}
		else if( m_run_state == S_RUN_STATE_WH_REACHED )
		{
		
		}
_end:
		usleep(1000*10);
	}
	m_quit = 1;
	PRINT_INFO("X s_wheel_t::run()\n");
	return 0;
}

