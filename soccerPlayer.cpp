//bob sang robot

#include "soccerPlayer.h"

s_soccer_player_t::s_soccer_player_t()
{
	m_arm = s_arm_t::get_inst();
	m_eye = s_eye_t::get_inst();
	m_wheel = s_wheel_t::get_inst();
}

s_soccer_player_t::~s_soccer_player_t()
{
}

s_soccer_player_t* s_soccer_player_t::get_inst()
{
	static s_soccer_player_t * g_soccer_player = NULL;
	if( !g_soccer_player )
		g_soccer_player = new s_soccer_player_t();

	return g_soccer_player;
}

int s_soccer_player_t::start()
{
	s_object_t::start();
	
	m_arm->start();
//	m_arm->standby(10);
	m_arm->right_low(10);

	m_eye->start();
	m_eye->standby(10);

	m_wheel->start();

	eye_start_search();
	
	return 0;
}

int s_soccer_player_t::stop()
{
	m_arm->stop();
	m_wheel->stop();
	m_eye->stop();

	m_run_state = S_RUN_STATE_NOTHING;
	s_object_t::stop();
	return 0;
}

int s_soccer_player_t::eye_start_search()
{
	PRINT_INFO("EYE START SEARCH ...");
	m_run_state = S_RUN_STATE_EYE_SEARCH_BALL;
	m_eye->search(10);
	return 0;
}

int s_soccer_player_t::eye_stop_search()
{
	PRINT_INFO("EYE STOP SEARCH\n\n");
	m_eye->stop_search();
	m_eye->go_pos_1(10);
	
	return 0;
}

int s_soccer_player_t::wheel_start_search()
{
	PRINT_INFO("WHEEL START SEARCH ...\n");
	m_wheel->start_search();
	return 0;
}

int s_soccer_player_t::wheel_stop_search()
{
	PRINT_INFO("WHEEL STOP SEARCH \n\n");
	m_wheel->stop_search();
	return 0;
}

int s_soccer_player_t::wheel_start_move()
{
	PRINT_INFO("WHEEL STAT MOVE\n");
	m_wheel->start_move();
	return 0;
}

int s_soccer_player_t::wheel_stop_move()
{
	PRINT_INFO("#### HIT IT #######\n");
	m_wheel->stop_move();
	return 0;
}

int s_soccer_player_t::arm_start_kick()
{
	PRINT_INFO("ARM START KICK\n");
	m_arm->kick(10);
	return 0;
}

int s_soccer_player_t::arm_stop_kick()
{
	m_arm->right_low(10);
	
	return 0;
}

int s_soccer_player_t::kick_ball()
{
	m_arm->right_low(10);
	
	return 0;
}


int s_soccer_player_t::run()
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
						m_run_state = S_RUN_STATE_EYE_RETURN_ORIG;
						eye_stop_search();
					}					
				}
				break;
			case S_RUN_STATE_EYE_RETURN_ORIG:
				if( m_eye->is_command_all_done() )
				{
					PRINT_INFO("EYE returned to original position, start wheel search \n");
					m_wheel->start_search();
					m_run_state = S_RUN_STATE_WHEEL_TURN;
				}
				break;				
				
			case S_RUN_STATE_EYE_SEARCH_DOOR:
				if(0 )// FIND_DOOR() )
				{
					m_run_state = S_RUN_STATE_WHEEL_TURN;
				}
				break;
			case S_RUN_STATE_WHEEL_TURN:
				{
					if( m_wheel->is_ball_found() )
					{
						PRINT_INFO("ball found, start to move to ball ..\n");
						m_run_state = S_RUN_STATE_WHEEL_MOVE_TO_BALL;
						m_wheel->start_move();
					}
				}
				break;
			case S_RUN_STATE_WHEEL_MOVE_TO_BALL:
				{
					if( m_wheel->is_ball_reached() )
					{
						PRINT_INFO("ball reached, start to kick ..\n");
						m_arm->kick(10);
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
//		WAIT_SIGNAL();
		usleep(1000*20);
	}
	return 0;
}

