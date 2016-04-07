//bob sang robot

#include "drivePlayer.h"


s_drive_player_t::s_drive_player_t()
{
	m_wheel = s_wheel_t::get_inst();

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

int s_drive_player_t::parse(char * msg)
{
	char * pbuf = strstr(msg, "count=");
	int count;
	int i;
	int alpha[20];
	int dis[20];
	pbuf += 6;
	count = atoi(pbuf);
	pbuf ++;
	printf("count =%d\n", count);
	for(i=0;i<count;i++)
	{
		pbuf = strstr(pbuf, "(");
		pbuf ++;
		alpha[i] = atoi(pbuf);
		pbuf ++;
		pbuf = strstr(pbuf, ",");
		pbuf ++;
		dis[i] = atoi(pbuf);
		pbuf ++;

		printf("alpha:%d dis:%d\n", alpha[i], dis[i]);
	}

	if( !is_started() )
		return 0;
	
	return 0;
}

int s_drive_player_t::on_location(char * msg)
{
	parse(msg);

	return 0;
}

int s_drive_player_t::start()
{
	s_object_t::start();

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

