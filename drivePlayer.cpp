//bob sang robot

#include "drivePlayer.h"


s_drive_player_t::s_drive_player_t()
{
}

s_drive_player_t::~s_drive_player_t()
{
}

s_drive_player_t* s_drive_player_t::get_inst()
{
	static s_drive_player_t * g_drive_player = NULL;
	if( !g_drive_player )
		g_drive_player = new s_drive_player_t();

	return g_drive_player;
}

int s_drive_player_t::play()
{

	return 0;
}

int s_drive_player_t::start()
{
	s_object_t::start();

	play();
	return 0;
}

int s_drive_player_t::stop()
{
	s_object_t::stop();
	
	return 0;
}

int s_drive_player_t::run()
{
	while(!m_quit)
	{
		LOCK_MUTEX(m_mutex);
		UNLOCK_MUTEX(m_mutex);
		usleep(1000*20);

	}
	m_quit = 1;
	return 0;
}

