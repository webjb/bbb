//bob sang robot

#include "drivePlayer.h"

#define NEXT_TURN_LEFT	1
#define NEXT_TURN_RIGHT 2

s_drive_player_t::s_drive_player_t()
{
	m_wheel = s_wheel_t::get_inst();
	m_eye = s_eye_t::get_inst();

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
	int alpha;
	int dis;
	int lr;
	pbuf += 6;
	count = atoi(pbuf);
	pbuf ++;
	printf("count =%d\n", count);
	s_lane_loc_t lane;
	memset(&lane, 0, sizeof(s_lane_loc_t));
	lane.m_left.m_dis = -1;
	lane.m_right.m_dis = -1;
	lane.m_top.m_dis = -1;
	
	for(i=0;i<count;i++)
	{
		pbuf = strstr(pbuf, "(");
		pbuf ++;
		alpha = atoi(pbuf);
		pbuf ++;
		pbuf = strstr(pbuf, ",");
		pbuf ++;
		dis = atoi(pbuf);
		pbuf ++;
		pbuf = strstr(pbuf, ",");
		pbuf ++;
		lr = atoi(pbuf);
		pbuf ++;
		
		if( alpha >= -40 && alpha < 40)
		{
			lane.m_top.m_alpha = alpha;
			lane.m_top.m_dis = dis;
		}
		else if( lr == 1 )
		{
			lane.m_right.m_alpha = alpha;
			lane.m_right.m_dis = dis;
		}
		else
		{
			lane.m_left.m_alpha = alpha;
			lane.m_left.m_dis = dis;
		}
		
	}
	printf("lane: right:(%d, %d) top:(%d,%d) left:(%d, %d)\n", 
		lane.m_right.m_alpha, lane.m_right.m_dis, 
		lane.m_top.m_alpha, lane.m_top.m_dis, 
		lane.m_left.m_alpha, lane.m_left.m_dis);

	if( !is_started() )
		return 0;

	drive(lane);
	
	return 0;
}

int s_drive_player_t::drive(s_lane_loc_t lane)
{
	m_loc_list.push_back(lane);

	if( lane.m_top.m_dis != -1 )
	{
		if( lane.m_right.m_dis != -1 )
			m_next_turn = NEXT_TURN_LEFT;
		else if( lane.m_left.m_dis != -1 )
			m_next_turn = NEXT_TURN_RIGHT;			
	}
	
 	if( lane.m_right.m_dis == -1 && lane.m_left.m_dis == -1 && lane.m_top.m_dis == -1)
	{
		m_wheel->move_stop();
		printf("drive stop\n");
		return 0;
	}

	if( (lane.m_top.m_dis < 200) && ( lane.m_top.m_dis != -1) )
	{
//		if( lane.m_right.m_dis != -1)
		if( m_next_turn == NEXT_TURN_LEFT )
		{
			m_wheel->move_forward_turn(10,15);
			printf("top turn left...\n");
		}
		else
		{
			m_wheel->move_forward_turn(15,10);
			printf("top turn right ...\n");
		}
	}
	else if( lane.m_right.m_dis < 200 )
	{
		m_wheel->move_forward_turn(10, 15);
		printf("drive turn left\n");
	}
	else if( lane.m_right.m_dis > 400 )
	{
		m_wheel->move_forward_turn(15,10);
		printf("drive turn right\n");
	}
	else
	{
		m_wheel->move_forward_turn(10,10);
		printf("drive forward\n");
	}
	
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
	m_eye->start();
	m_eye->standby(10);
	

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

