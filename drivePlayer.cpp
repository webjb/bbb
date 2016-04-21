//bob sang robot

#include "drivePlayer.h"

#include <math.h>

#define LANE_TYPE_UNKNOWN 0
#define LANE_TYPE_LEFT	1
#define LANE_TYPE_RIGHT	2
#define LANE_TYPE_LEFT_TOP	3
#define LANE_TYPE_RIGHT_TOP 4

s_drive_player_t::s_drive_player_t()
{
	m_wheel = s_wheel_t::get_inst();
	m_eye = s_eye_t::get_inst();

	memset(&m_lane_left, 0, sizeof(m_lane_left));
	memset(&m_lane_right, 0, sizeof(m_lane_right));
	memset(&m_lane_left_top, 0, sizeof(m_lane_left_top));
	memset(&m_lane_right_top, 0, sizeof(m_lane_right_top));

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

int get_points_distance(s_point_t p0, s_point_t p1)
{
	float dis;
	float fx;
	float fy;

	fx = (float)(p0.m_x - p1.m_x);
	fx = fx*fx;

	fy = (float)(p0.m_y - p1.m_y);
	fy = fy*fy;

	dis = sqrt(fx + fy);
	return (int)dis;
	
}

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

		if( abs(pline_group[i]->m_avg_alpha) < 45 )
		{
			for( j=0;j<pline_group[i]->m_count; j++)
			{
				s_line *pline = &pline_group[i]->m_lines[j];
					
				avg_alpha += pline->m_alpha;
				avg_dis += pline->m_dis;

				if( point_top.m_x < pline->m_start.m_x)
				{
					point_top.m_y = pline->m_start.m_y;
					point_top.m_x = pline->m_start.m_x;
				}
				
				if( point_top.m_x < pline->m_end.m_x)
				{
					point_top.m_y = pline->m_end.m_y;
					point_top.m_x = pline->m_end.m_x;
				}
				
				if( point_bottom.m_x > pline->m_start.m_x)
				{
					point_bottom.m_y = pline->m_start.m_y;
					point_bottom.m_x = pline->m_start.m_x;
				}
				
				if( point_bottom.m_x > pline->m_end.m_x)
				{
					point_bottom.m_y = pline->m_end.m_y;
					point_bottom.m_x = pline->m_end.m_x;
				}
				
				printf("(%d,%d)-(%d,%d) alpha:%d dis:%d lr:%d\n", pline->m_start.m_x, pline->m_start.m_y, pline->m_end.m_x, pline->m_end.m_y,
					pline->m_alpha, pline->m_dis, pline->m_lr);
			}
		
		}
		else
		{
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
		}
		avg_alpha /= pline_group[i]->m_count;
		avg_dis /= pline_group[i]->m_count;

		pline_group[i]->m_avg_alpha = avg_alpha;
		pline_group[i]->m_avg_dis = avg_dis;
		
		pline_group[i]->m_point_bot.m_x = point_bottom.m_x;
		pline_group[i]->m_point_bot.m_y = point_bottom.m_y;
		
		pline_group[i]->m_point_top.m_x = point_top.m_x;
		pline_group[i]->m_point_top.m_y = point_top.m_y;
		
		printf("lr:%d count:%d avg_alpha:%d avg_dis:%d bot:(%d, %d) top:(%d,%d)\n", pline_group[i]->m_lr, pline_group[i]->m_count, avg_alpha, avg_dis,
			point_bottom.m_x, point_bottom.m_y,point_top.m_x, point_top.m_y);
		
	}
	
	printf("\n");

	detect_direction(pline_group);
	
	for( i=0;i<MAX_LINE_GROUP;i++)
		delete pline_group[i];
	
	return 0;
}


int s_drive_player_t::detect_direction(s_line_group * pline_group[])
{
	int i;

	m_lane_right.m_count = -1;
	m_lane_left.m_count = -1;
	m_lane_right_top.m_count = -1;
	m_lane_left_top.m_count = -1;
		
	// find right
	for( i=0;i<MAX_LINE_GROUP;i++)
	{
		if( pline_group[i]->m_count <= 0 )
			continue;

		if( pline_group[i]->m_avg_alpha < 45 && pline_group[i]->m_avg_alpha > -45)
		{
		}			
		else if( (pline_group[i]->m_lr == 1) )
		{
			if( m_lane_right.m_count <= 0 )
			{
				memcpy(&m_lane_right, pline_group[i], sizeof(m_lane_right));
			}
			else if( m_lane_right.m_avg_dis > pline_group[i]->m_avg_dis)
			{
				memcpy(&m_lane_right, pline_group[i], sizeof(m_lane_right));
			}				
		}
	}
	// find left
	for( i=0;i<MAX_LINE_GROUP;i++)
	{
		if( pline_group[i]->m_count <= 0 )
			continue;
	
		if( pline_group[i]->m_avg_alpha < 45 && pline_group[i]->m_avg_alpha > -45)
		{
		}			
		else if( (pline_group[i]->m_lr == 0) )
		{
			if( m_lane_left.m_count <= 0 )
			{
				memcpy(&m_lane_left, pline_group[i], sizeof(m_lane_left));
			}
			else if( m_lane_left.m_avg_dis > pline_group[i]->m_avg_dis)
			{
				memcpy(&m_lane_left, pline_group[i], sizeof(m_lane_left));
			}				
		}
		
	}
	
	// find right top	
	// find left top
	for( i=0;i<MAX_LINE_GROUP;i++)
	{
		s_line_group * g;
		g = pline_group[i];
		if( g->m_count <= 0 )
			continue;

		if( abs(g->m_avg_alpha) < 45)
		{
			int dis0 = -1;
			int dis1 = -1;
			int dis2 = -1;
			int dis3 = -1;

			if( g->m_point_top.m_x < g->m_point_bot.m_x )
			{
				int tmp_x;
				int tmp_y;

				tmp_x = g->m_point_top.m_x;
				tmp_y = g->m_point_top.m_y;

				g->m_point_top.m_x = g->m_point_bot.m_x;
				g->m_point_top.m_y = g->m_point_bot.m_y;

				g->m_point_bot.m_x = tmp_x;
				g->m_point_bot.m_y = tmp_y;
				
			}

			if( m_lane_left.m_count > 0)
			{
				dis0 = get_points_distance(pline_group[i]->m_point_top, m_lane_left.m_point_top);
				dis1 = get_points_distance(pline_group[i]->m_point_bot, m_lane_left.m_point_top);

				if( dis0 > dis1 )
					dis0 = dis1;
			}
			
			if( m_lane_right.m_count > 0)
			{
				dis2 = get_points_distance(pline_group[i]->m_point_top, m_lane_right.m_point_top);
				dis3 = get_points_distance(pline_group[i]->m_point_bot, m_lane_right.m_point_top);

				if( dis2 > dis3)
					dis2 = dis3;
			}

			if( (dis0 == -1) && (dis2 == -1) )
			{
				if( m_lane_left_top.m_count > 0)
				{
					memcpy(&m_lane_right_top, pline_group[i], sizeof(m_lane_right_top));
				}
				else
				{
					memcpy(&m_lane_left_top, pline_group[i], sizeof(m_lane_left_top));
				}
				
			}
			else if( (dis0 == -1 ) && (dis2 >= 0 ))
			{
				memcpy(&m_lane_right_top, pline_group[i], sizeof(m_lane_right_top));				
			}
			else if( (dis0 >= 0 ) && (dis2 == -1 ))
			{
				memcpy(&m_lane_left_top, pline_group[i], sizeof(m_lane_left_top));				
			}
			else if( dis0 > dis2 )
			{
				memcpy(&m_lane_right_top, pline_group[i], sizeof(m_lane_right_top));								
			}
			else
			{
				memcpy(&m_lane_left_top, pline_group[i], sizeof(m_lane_left_top));				
			}						
		}			
	}

	for( i=0; i<4; i++)
	{
		s_line_group *lg;
		char sz[80];
		
		switch (i)
		{
			case 0:
				sprintf(sz, "left: ");
				lg = &m_lane_left;
				break;
			case 1:
				sprintf(sz, "right: ");
				lg = &m_lane_right;
				break;
			case 2:
				sprintf(sz, "left_top: ");
				lg = &m_lane_left_top;
				break;
			case 3:
				sprintf(sz, "right_top: ");
				lg = &m_lane_right_top;
				break;
		}
		if( lg->m_count > 0 )
		{
			printf("--- %s --- count:%d alpha:%d dis:%d bot:(%d, %d) top:(%d,%d)\n", sz, lg->m_count, lg->m_avg_alpha, lg->m_avg_dis, lg->m_point_bot.m_x, lg->m_point_bot.m_y, lg->m_point_top.m_x, lg->m_point_top.m_y);
		}
		
	}

	return 0;
}

int s_drive_player_t::drive()
{
	int right_height;
	int left_height;
	
 	if( (m_lane_left.m_count <= 0) 
		&& (m_lane_right.m_count <= 0)
		&& (m_lane_left_top.m_count <= 0)
		&& (m_lane_right_top.m_count <= 0)
		)
	{
		m_wheel->move_stop();
		m_run_state = DRV_STATE_STOP;
		
		printf("drive stop\n");
		return 0;
	}


	right_height = -1;
	left_height = -1;
	
	if( m_run_state == DRV_STATE_FWD)
	{
		int d1;
		int d2;
		if( (m_lane_right.m_count > 0) &&  (m_lane_right_top.m_count > 0) )
		{
			d1 = abs(m_lane_right_top.m_point_bot.m_x - m_lane_right.m_point_top.m_x);
			d2 = abs(m_lane_right_top.m_point_top.m_x - m_lane_right.m_point_top.m_x);
			if( d1 < d2 )
			{
				m_next_turn = NEXT_TURN_RIGHT;
				printf("next turn right ...\n");
			}
			else
			{
				m_next_turn = NEXT_TURN_LEFT;
				printf("next turn left ...\n");
			}
			right_height = m_lane_right_top.m_avg_dis;
		}

		if( (m_lane_left.m_count > 0) &&  (m_lane_left_top.m_count > 0) )
		{
			d1 = abs(m_lane_left_top.m_point_bot.m_x - m_lane_left.m_point_top.m_x);
			d2 = abs(m_lane_left_top.m_point_top.m_x - m_lane_left.m_point_top.m_x);
			if( d1 < d2 )
			{
				m_next_turn = NEXT_TURN_RIGHT;
				printf("next turn right ###\n");
			}
			else
			{
				m_next_turn = NEXT_TURN_LEFT;
				printf("next turn left ###\n");
			}

			left_height = m_lane_left_top.m_avg_dis;
		}

		if( m_next_turn == NEXT_TURN_LEFT )
		{
			if( ((right_height <= 350 ) && (right_height > 0)) || (m_lane_right.m_count <= 0) )
			{
				m_next_turn = 0;
				m_wheel->move_forward_turn(10,25);
				m_run_state = DRV_STATE_TURN_LEFT;
				m_last_time = s_timer_t::get_inst()->get_ms();
				printf("top turn left dis:%d...\n", right_height);
			}
		}
		else if( m_next_turn == NEXT_TURN_RIGHT )
		{
			if( ((left_height <= 350 ) && (left_height > 0)) || (m_lane_left.m_count <= 0) )
			{
				m_next_turn = 0;
				m_wheel->move_forward_turn(25, 10);
				m_last_time = s_timer_t::get_inst()->get_ms();
				m_run_state = DRV_STATE_TURN_RIGHT;
				printf("top turn right dis:%d...\n", left_height);
			}
		}
	
	}

	
	if( m_run_state == DRV_STATE_TURN_LEFT )
	{
		int64 ms;
		ms = s_timer_t::get_inst()->get_ms();
		ms = ms - m_last_time;
		if( ( ms > 3000) && (m_lane_right.m_count > 0 ) && (abs(m_lane_right.m_avg_alpha) > 65) )
		{
			m_run_state = DRV_STATE_FWD;
			m_wheel->move_forward(15);
			printf("turn left ---> forward\n");
		}
		return 0;
	}

	if( m_run_state == DRV_STATE_TURN_RIGHT )
	{
		int64 ms;
		ms = s_timer_t::get_inst()->get_ms();
		ms = ms - m_last_time;
		if(  ( ms > 3000) && (m_lane_left.m_count > 0 ) &&  (abs(m_lane_left.m_avg_alpha) > 65) )
		{
			m_run_state = DRV_STATE_FWD;
			m_wheel->move_forward(15);
			printf("turn right ---> forward\n");
		}
		return 0;
	}

	m_run_state = DRV_STATE_FWD;

	if( (m_lane_right.m_count > 0 ) && (m_lane_left.m_count > 0 ) )
	{
		int d;
		d = (m_lane_right.m_avg_dis + m_lane_left.m_avg_dis)/2;
		if( m_lane_right.m_avg_dis > d + 50 ) // turn right
		{
			m_wheel->move_forward_turn(15, 10);
			printf("drive turn right ...\n");
		}
		else if( m_lane_right.m_avg_dis < d - 50 ) // turn left
		{
			m_wheel->move_forward_turn(10, 15);
			printf("drive turn left ...\n");
		}
		else
		{
			m_wheel->move_forward(15);
			printf("drive forward ...\n");
		}
	}
	else
	{
		if( m_lane_right.m_avg_dis < 200 )
		{
			m_wheel->move_forward_turn(10, 15);
			printf("drive turn left\n");
		}
		else if( m_lane_right.m_avg_dis > 400 )
		{
			m_wheel->move_forward_turn(15,10);
			printf("drive turn right\n");
		}
		else
		{
			m_wheel->move_forward(15);
			printf("drive forward\n");
		}
	}
	
	
	return 0;
}

int s_drive_player_t::drive(s_lane_loc_t lane)
{
#if 0
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
#endif	
	return 0;
}

int s_drive_player_t::on_location(char * msg)
{
	parse(msg);
	
	if( !is_started() )
		return 0;

	drive();

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

