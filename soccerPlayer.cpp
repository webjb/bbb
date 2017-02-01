//bob sang robot

#include "soccerPlayer.h"
#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

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
	m_arm->right_low(10);

	m_wheel->start();

	eye_start_search();
	return 0;
}

int s_soccer_player_t::stop()
{
	s_log_info("soccer_player stop\n");
	m_arm->stop();
	m_wheel->stop();
//	m_eye->stop();

	m_run_state = S_RUN_STATE_NOTHING;
	s_object_t::stop();
	return 0;
}

int s_soccer_player_t::eye_start_search()
{
	s_ball_postion_t pos;
	s_log_info("EYE START SEARCH ...");
	
	m_eye->get_ball_position( &pos );
	if( pos.m_distance < 0 )
	{
		m_eye->search(10);
		m_run_state = S_RUN_STATE_EYE_SEARCH_BALL;
	}
	else	
	{
		m_wheel->start_move();
		m_run_state = S_RUN_STATE_WHEEL_MOVE_TO_BALL;
	}
	return 0;
}

int s_soccer_player_t::eye_stop_search()
{
	s_log_info("EYE STOP SEARCH\n\n");
	m_eye->stop_search();
	m_eye->go_pos_1(10);
	
	return 0;
}
/*
int s_soccer_player_t::wheel_start_search()
{
	s_log_info("WHEEL START SEARCH ...\n");
	m_wheel->start_search();
	return 0;
}

int s_soccer_player_t::wheel_stop_search()
{
	s_log_info("WHEEL STOP SEARCH \n\n");
	m_wheel->stop_search();
	return 0;
}
*/
int s_soccer_player_t::wheel_start_move()
{
	s_log_info("WHEEL STAT MOVE\n");
	m_wheel->start_move();
	return 0;
}

int s_soccer_player_t::wheel_stop_move()
{
	s_log_info("#### HIT IT #######\n");
	m_wheel->stop_move();
	return 0;
}

int s_soccer_player_t::arm_start_kick()
{
	s_log_info("ARM START KICK\n");
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
	int x,y;

	x = 0;
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
					if( pos.m_distance > 0 )
					{
						s_log_info("found ball, eye return back\n");
						m_run_state = S_RUN_STATE_EYE_RETURN_ORIG;
						m_eye->stop_search();
						m_eye->get_moved_xy(&x, &y);
						m_eye->go_pos_1(10);
					}					
				}
				break;
			case S_RUN_STATE_EYE_RETURN_ORIG:
				if( m_eye->is_command_all_done() )
				{					
					s_log_info("EYE returned to original position, start wheel search \n");
					m_wheel->start_search(x);
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
						s_log_info("ball found, start to move to ball ..\n");
						m_run_state = S_RUN_STATE_WHEEL_MOVE_TO_BALL;
						m_wheel->start_move();
					}
				}
				break;
			case S_RUN_STATE_WHEEL_MOVE_TO_BALL:
				{
					if( m_wheel->is_ball_reached() )
					{
						s_log_info("ball reached, start to aim to door ..\n");
//						m_arm->kick(10);
//						m_run_state = S_RUN_STATE_ARM_KICK;
						m_wheel->start_aim_door();
						m_run_state = S_RUN_STATE_WHEEL_AIM_TO_DOOR;
					}
				}
				break;
			case S_RUN_STATE_WHEEL_AIM_TO_DOOR:
				if( m_wheel->is_door_aimed())
				{
					m_arm->kick(10);
					m_run_state = S_RUN_STATE_ARM_KICK;
				}
				else if( m_wheel->is_ball_lost() )
				{
					s_ball_postion_t pos;
					s_log_info("ball is lost, start over\n");
					m_eye->get_ball_position( &pos );
					
					if( pos.m_distance < 0 )
					{
						eye_start_search();
					}
					else
					{
						m_run_state = S_RUN_STATE_EYE_SEARCH_BALL;
					}
				}
				break;
			case S_RUN_STATE_ARM_KICK:
				if( m_arm->is_command_all_done() )
				{
					s_log_info("kick is done\n");
					m_run_state = S_RUN_STATE_NOTHING;
				}
				break;
				
	 	}
//		WAIT_SIGNAL();
		s_sleep_ms(20);
	}
	return 0;
}

