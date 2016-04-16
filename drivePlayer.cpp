//bob sang robot

#include "drivePlayer.h"

#include <math.h>

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

void to_axis(int width, int height, s_point_t & p)
{
	p.m_x = p.m_x - width/2;
	p.m_y = height - p.m_y;
}


#define PI 3.14159

int calc_distance(int width, int height, s_point_t p0, s_point_t p1, float & alpha, int & lr ) 
{
    int x0;
    int y0;
    int x1;
    int y1;
    int x_org;
    int y_org;
    float a;
    float b;

    lr = 0;
    x_org = width/2;
    y_org = height;

    x0 = p0.m_x - x_org;
    y0 = y_org - p0.m_y;

    x1 = p1.m_x - x_org;
    y1 = y_org - p1.m_y;

    int d;

    if( y1 == y0)
    {
        alpha = 0;
        d = y1;
    }
    else if( (x1 != x0) && (x1 != 0) ) {
        a = (float) (y1 - y0);
        a = a / ((float) (x1 - x0));
        b = y0 - a * (float) x0;
        alpha = atan(a);
        d = (int)((b/a) * sin(alpha));

        alpha *= 180/PI;
        if( ((b>0) && (a<0)) || ((b<0)&& (a>0)))
        {
            lr = 1;
        }
        else
        {
            lr = 0;
        }

    }
    else {
        a = 0;
        b = 0;
        alpha = 90;
        d = x1;
        if( d > 0 ) {
            lr = 1;
        }
        else
        {
            lr = 0;
        }
    }

    d = abs(d);
    return d;
}


int s_drive_player_t::parse(char * msg)
{
	char * pbuf = strstr(msg, "count=");
	int count;
	int i;
	int width;
	int height;
	int len;
	int num[200];
	int num_count;
	int last_is_num;
	int k;

	s_point_t points[100];
	int j;
	
	s_line_group *pline_group[MAX_LINE_GROUP];

	num_count = 0;
	num[0] = 0;
	len = strlen(pbuf);
	last_is_num = 0;
	while( len-- )
	{
		if( (*pbuf <= '9') && (*pbuf >= '0') )
		{
			num[num_count] *= 10;
			num[num_count] += *pbuf - '0';
			last_is_num = 1;
		}
		else
		{
			if( last_is_num == 1 )
			{
				num_count ++;
				num[num_count] = 0;
			}
			last_is_num = 0;
		}
		pbuf ++;
	}
	if( last_is_num == 1 )
		num_count ++;
	
	if( num_count > 100 )
		return 0;

	i = 0;
	count = num[i++];
	width = num[i++];
	height = num[i++];

	printf("count:%d size:%d-%d \n", count, width, height);

	if( count == 0 )
		return 0;
	
	for( k=0;k<MAX_LINE_GROUP;k++)
	{
		pline_group[k] = new s_line_group;
		pline_group[k]->m_count = 0;
		pline_group[k]->m_avg_dis = -1;
			
		pline_group[k]->m_avg_alpha = -1000;		
	}
	for( j=0;j<count;j++)
	{
		int dis;
		int lr;
		float alpha;
		points[2*j].m_x = num[i++];
		points[2*j].m_y = num[i++];
		points[2*j+1].m_x = num[i++];
		points[2*j+1].m_y = num[i++];
		dis = calc_distance(width, height, points[2*j], points[2*j+1], alpha, lr);
		
		to_axis(width, height, points[2*j]);
		to_axis(width, height, points[2*j+1]);

		int findit = 0;
		for( k=0;k<MAX_LINE_GROUP;k++)
		{
			if( pline_group[k]->m_avg_dis == -1 )
				continue;
			if( (lr == pline_group[k]->m_lr ) 
				&& ( dis > pline_group[k]->m_avg_dis - 100)
				&& ( dis < pline_group[k]->m_avg_dis + 100)
				&& ( (int)alpha > pline_group[k]->m_avg_alpha-10 ) 
				&& ( (int)alpha < pline_group[k]->m_avg_alpha+10 )
				)
			{
				s_line *pline = &pline_group[k]->m_lines[pline_group[k]->m_count];
				pline->m_alpha = alpha;
				pline->m_dis = dis;
				pline->m_lr = lr;
				pline->m_start.m_x = points[2*j].m_x;
				pline->m_start.m_y = points[2*j].m_y;
				
				pline->m_end.m_x = points[2*j+1].m_x;
				pline->m_end.m_y = points[2*j+1].m_y;
				
				pline_group[k]->m_count ++;
				pline_group[k]->m_avg_alpha = (int)alpha;
				pline_group[k]->m_avg_dis = dis;
				pline_group[k]->m_lr = lr;
				findit = 1;
				break;
			}

		}
		
		if ( !findit )
		{
			for( k=0;k<MAX_LINE_GROUP;k++)
			{
				if( pline_group[k]->m_count <= 0 )
				{
					break;
				}
			}
			s_line *pline = &pline_group[k]->m_lines[0];
			pline->m_alpha = alpha;
			pline->m_dis = dis;
			pline->m_lr = lr;
			pline->m_start.m_x = points[2*j].m_x;
			pline->m_start.m_y = points[2*j].m_y;
			
			pline->m_end.m_x = points[2*j+1].m_x;
			pline->m_end.m_y = points[2*j+1].m_y;
						
			pline_group[k]->m_count = 1;
			pline_group[k]->m_avg_alpha = (int)alpha;
			pline_group[k]->m_avg_dis = dis;
			pline_group[k]->m_lr = lr;
		}
		
		printf(" (%d,%d)-(%d,%d) alpha:%d dis:%d lr:%d\n", points[2*j].m_x, points[2*j].m_y, points[2*j+1].m_x, points[2*j+1].m_y, (int)alpha, dis, lr);
	}
	printf("Line Groups\n");
	for( i=0;i<MAX_LINE_GROUP;i++)
	{
		if( pline_group[i]->m_count == 0 )
			break;
		printf("group: count:%d \n", pline_group[i]->m_count);
		int avg_alpha = 0;
		int avg_dis = 0;
		s_point_t point_top;
		s_point_t point_bottom;

		point_bottom.m_x = pline_group[i]->m_lines[0].m_start.m_x;
		point_bottom.m_y = pline_group[i]->m_lines[0].m_start.m_y;
		point_top.m_x = point_bottom.m_x;
		point_top.m_y = point_bottom.m_y;
		
		for( j=0;j<pline_group[i]->m_count; j++)
		{
			s_line *pline = &pline_group[i]->m_lines[j];
				
			avg_alpha += pline->m_alpha;
			avg_dis += pline->m_dis;

			if( point_top.m_y < pline->m_start.m_y)
			{
				point_top.m_y = pline->m_start.m_y;
				point_top.m_x = pline->m_start.m_x;
			}
			
			if( point_top.m_y < pline->m_end.m_y)
			{
				point_top.m_y = pline->m_end.m_y;
				point_top.m_x = pline->m_end.m_x;
			}
			
			if( point_bottom.m_y > pline->m_start.m_y)
			{
				point_bottom.m_y = pline->m_start.m_y;
				point_bottom.m_x = pline->m_start.m_x;
			}
			
			if( point_bottom.m_y > pline->m_end.m_y)
			{
				point_bottom.m_y = pline->m_end.m_y;
				point_bottom.m_x = pline->m_end.m_x;
			}
			
			printf("(%d,%d)-(%d,%d) alpha:%d dis:%d lr:%d\n", pline->m_start.m_x, pline->m_start.m_y, pline->m_end.m_x, pline->m_end.m_y,
				pline->m_alpha, pline->m_dis, pline->m_lr);
		}
		avg_alpha /= pline_group[i]->m_count;
		avg_dis /= pline_group[i]->m_count;

		pline_group[i]->m_avg_alpha = avg_alpha;
		pline_group[i]->m_avg_dis = avg_dis;
		
		pline_group[i]->m_point_bot.m_x = point_bottom.m_x;
		pline_group[i]->m_point_bot.m_y = point_bottom.m_y;
		
		pline_group[i]->m_point_top.m_x = point_top.m_x;
		pline_group[i]->m_point_top.m_y = point_top.m_y;
		
		printf(" avg_alpha:%d avg_dis:%d bot:(%d, %d) top:(%d,%d)\n", avg_alpha, avg_dis,
			point_bottom.m_x, point_bottom.m_y,point_top.m_x, point_top.m_y);
		
	}
	
	for( i=0;i<MAX_LINE_GROUP;i++)
		delete pline_group[i];
	printf("\n");

	
	if( !is_started() )
		return 0;

	drive(pline_group);
	
	return 0;
}

#if 0
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
		int data[7];
		for( int j=0; j<7; j++)
		{
			if( j==0 )
			{
				pbuf = strstr(pbuf, "(");
			}
			else
			{
				pbuf = strstr(pbuf, ",");
			}
			pbuf ++;
			data[j] = atoi(pbuf);
		}
		
		alpha = data[4];
		dis = data[5];
		lr = data[6];
		
		if( alpha >= -50 && alpha < 50)
		{
			lane.m_top.m_alpha = alpha;
			lane.m_top.m_dis = dis;
			lane.m_top.m_points[0].m_x = data[0];
			lane.m_top.m_points[0].m_y = data[1];
			lane.m_top.m_points[1].m_x = data[2];
			lane.m_top.m_points[1].m_y = data[3];
		}
		else if( lr == 1 )
		{
			lane.m_right.m_alpha = alpha;
			lane.m_right.m_dis = dis;
			lane.m_right.m_points[0].m_x = data[0];
			lane.m_right.m_points[0].m_y = data[1];
			lane.m_right.m_points[1].m_x = data[2];
			lane.m_right.m_points[1].m_y = data[3];
		}
		else
		{
			lane.m_left.m_alpha = alpha;
			lane.m_left.m_dis = dis;
			lane.m_left.m_points[0].m_x = data[0];
			lane.m_left.m_points[0].m_y = data[1];
			lane.m_left.m_points[1].m_x = data[2];
			lane.m_left.m_points[1].m_y = data[3];
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
#endif

int s_drive_player_t::drive(s_line_group * pline_group[])
{

	return 0;
}

int s_drive_player_t::drive(s_lane_loc_t lane)
{
	m_loc_list.push_back(lane);

	if( m_run_state == DRV_STATE_FWD )
	{
		if( lane.m_top.m_dis != -1 )
		{
			if( lane.m_right.m_dis != -1 )
			{
				if( lane.m_right.m_points[0].m_x > lane.m_top.m_points[0].m_x)
				{
					m_next_turn = NEXT_TURN_LEFT;
				}
				else
				{
					m_next_turn = NEXT_TURN_RIGHT;
				}
			}
		}
	}
	
 	if( lane.m_right.m_dis == -1 && lane.m_left.m_dis == -1 && lane.m_top.m_dis == -1)
	{
		m_wheel->move_stop();
		m_run_state = DRV_STATE_STOP;
		
		printf("drive stop\n");
		return 0;
	}

	if( m_run_state == DRV_STATE_FWD )
	{
		if( (lane.m_top.m_dis < 300) && ( lane.m_top.m_dis != -1) )
		{
			if( m_next_turn == NEXT_TURN_LEFT )
			{
				m_wheel->move_forward_turn(10,25);
				m_run_state = DRV_STATE_TURN_LEFT;
				printf("top turn left...\n");
			}
			else
			{
				m_wheel->move_forward_turn(25,10);
				m_run_state = DRV_STATE_TURN_RIGHT;
				printf("top turn right ...\n");
			}
		}
	}
	
	if( m_run_state == DRV_STATE_TURN_LEFT )
	{
		if( ( (lane.m_right.m_dis != -1) && abs(lane.m_right.m_alpha) > 72) 
			|| 
			( (lane.m_right.m_dis != -1) && (lane.m_left.m_dis != -1) ) )
		  
		{
			m_run_state = DRV_STATE_FWD;
		}
		else
			return 0;
	}
	m_run_state = DRV_STATE_FWD;

	if( (lane.m_right.m_dis != -1) && (lane.m_left.m_dis != -1) )
	{
		int d;
		d = (lane.m_right.m_dis + lane.m_left.m_dis)/2;
		if( lane.m_right.m_dis > d + 50 ) // turn right
		{
			m_wheel->move_forward_turn(15, 10);
			printf("drive turn right ...\n");
		}
		else if( lane.m_right.m_dis < d - 50 ) // turn left
		{
			m_wheel->move_forward_turn(10, 15);
			printf("drive turn left ...\n");
		}
		else
		{
			m_wheel->move_forward_turn(15,15);
			printf("drive forward ...\n");
		}
	}
	else
	{
		if( lane.m_right.m_dis < 200 )
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
			m_wheel->move_forward_turn(15,15);
			printf("drive forward\n");
		}
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
	
	m_run_state = DRV_STATE_STOP;
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

