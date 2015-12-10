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
	while(!this->m_quit)
	{
		if( m_exit == 1)
			break;
	 	switch( m_run_state )
		{
			case S_RUN_STATE_EYE_SEARCH_BALL:
				{
					s_ball_postion_t pos;
					m_eye->get_ball_position( &pos );
					if( FOUND_BALL(pos.m_x, pos.m_y) )
					{
						PRINT_INFO("found ball, eye return back\n");
						m_run_state = S_RUN_STATE_ARM_KICK;
					}					
				}
				break;
				
			case S_RUN_STATE_ARM_KICK:
				if( m_arm->is_command_all_done() )
				{
					PRINT_INFO("kick is done\n");
					m_run_state = S_RUN_STATE_NOTHING;
				}
				break;
				
	 	}
		usleep(1000*20);
	}
	return 0;
}

