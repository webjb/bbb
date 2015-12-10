// bob sang for Neza Robot System

#include "movingPlayer.h"

s_moving_player_t::s_moving_player_t()
{
	m_arm = s_arm_t::get_inst();
	m_eye = s_eye_t::get_inst();
	m_wheel = s_wheel_t::get_inst();
}

s_moving_player_t::~s_moving_player_t()
{
}

s_moving_player_t* s_moving_player_t::get_inst()
{
	static s_moving_player_t * g_moving_player = NULL;
	if( !g_moving_player )
		g_moving_player = new s_moving_player_t();

	return g_moving_player;
}

int s_moving_player_t::start()
{
	s_object_t::start();
	
	m_arm->start();
	m_arm->right_low(10);

	m_eye->start();
	m_eye->standby(10);

	m_wheel->start();
	
	return 0;
}

int s_moving_player_t::stop()
{
	m_arm->stop();
	m_wheel->stop();
	m_eye->stop();

	m_run_state = S_RUN_STATE_NOTHING;
	s_object_t::stop();
	return 0;
}


int s_moving_player_t::arm_start_kick()
{
	PRINT_INFO("ARM START KICK\n");
	m_arm->kick(10);
	return 0;
}

int s_moving_player_t::arm_stop_kick()
{
	m_arm->right_low(10);
	
	return 0;
}

int s_moving_player_t::kick_ball()
{
	m_arm->right_low(10);
	
	return 0;
}

int s_moving_player_t::run()
{
	s_ball_postion_t pos;
	int x_speed = -1;
//	int y_speed = -1;
	int x_pos = -1;
//	int y_pos = -1;
	int64 last_time;
	while(!this->m_quit)
	{
		if( m_exit == 1)
			break;
		if( m_run_state == S_RUN_STATE_EYE_SEARCH_BALL)
		{
			m_eye->get_ball_position( &pos );
			if( FOUND_BALL(pos.m_x, pos.m_y) )
			{
				last_time = s_timer_t::get_inst()->get_ms();
				if( x_pos == -1 )
				{
					x_pos = pos.m_x;
//					y_pos = pos.m_y;
				}
				else
				{
					int delt_time = 1;
					delt_time = (int) (s_timer_t::get_inst()->get_ms() - last_time);
					
					x_speed = (pos.m_x - x_pos)*1000/delt_time;
//					y_speed = (pos.m_y - y_pos)*1000/delt_time;

					delt_time = abs(pos.m_x) / x_speed;
					if( delt_time < 2000 )
					{
						// kick the ball
						m_arm->kick(10);
					}
					m_run_state = S_RUN_STATE_ARM_KICK;
				}
			}
		}
		
		if( m_run_state == S_RUN_STATE_ARM_KICK)
		{
			if( s_timer_t::get_inst()->get_ms() - last_time > 4000)
			{
				m_run_state = S_RUN_STATE_EYE_SEARCH_BALL;
				x_speed = -1;
				//y_speed = -1;
				x_pos = -1;
//				y_pos = -1; 			
			}
		}
		usleep(1000*20);
	}
	return 0;
}

