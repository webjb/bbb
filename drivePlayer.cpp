//bob sang robot

#include "drivePlayer.h"
#include "utilities.h"
#include "log.h"
#include <math.h>

#define TIMEOUT_TURNING_MS	3000

using namespace s_utilities;
using namespace s_log;

s_drive_player_t::s_drive_player_t()
{
	m_wheel = s_wheel_t::get_inst();
	m_eye = s_eye_t::get_inst();

	s_memset(&m_lane_left, 0, sizeof(m_lane_left));
	s_memset(&m_lane_right, 0, sizeof(m_lane_right));
	s_memset(&m_lane_left_top, 0, sizeof(m_lane_left_top));
	s_memset(&m_lane_right_top, 0, sizeof(m_lane_right_top));

    CREATE_MUTEX(m_mutex);	
    CREATE_SIGNAL(m_loc_signal, 0);	
}

s_drive_player_t::~s_drive_player_t()
{
	DESTROY_SIGNAL(m_loc_signal);
    DESTROY_MUTEX(m_mutex);

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
    else if( x1 != x0 ) {
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

	s_log_info("count:%d size:%d-%d \n", count, width, height);

	
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
		
		s_log_info(" (%d,%d)-(%d,%d) alpha:%d dis:%d lr:%d\n", points[2*j].m_x, points[2*j].m_y, points[2*j+1].m_x, points[2*j+1].m_y, (int)alpha, dis, lr);
	}
	s_log_info("Line Groups\n");
	for( i=0;i<MAX_LINE_GROUP;i++)
	{
		if( pline_group[i]->m_count == 0 )
			break;
		s_log_info("group: count:%d \n", pline_group[i]->m_count);
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
				
				s_log_info("(%d,%d)-(%d,%d) alpha:%d dis:%d lr:%d\n", pline->m_start.m_x, pline->m_start.m_y, pline->m_end.m_x, pline->m_end.m_y,
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
				
				s_log_info("(%d,%d)-(%d,%d) alpha:%d dis:%d lr:%d\n", pline->m_start.m_x, pline->m_start.m_y, pline->m_end.m_x, pline->m_end.m_y,
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

		if( abs(avg_alpha) < 45)
			pline_group[i]->m_type = LANE_TYPE_TOP_UNKNOWN;
		else
			pline_group[i]->m_type = LANE_TYPE_SIDE_UNKNOWN;
		
		s_log_info("lr:%d count:%d avg_alpha:%d avg_dis:%d bot:(%d, %d) top:(%d,%d)\n", pline_group[i]->m_lr, pline_group[i]->m_count, avg_alpha, avg_dis,
			point_bottom.m_x, point_bottom.m_y,point_top.m_x, point_top.m_y);
		
	}
	
	s_log_info("\n");

	detect_direction(pline_group);
	
	for( i=0;i<MAX_LINE_GROUP;i++)
		delete pline_group[i];
	
	return 0;
}


int s_drive_player_t::detect_direction(s_line_group * pline_group[])
{
	int i;
	s_line_group lane_right;
	s_line_group lane_left;
	s_line_group lane_right_top;
	s_line_group lane_left_top;

	lane_right.m_count = -1;
	lane_left.m_count = -1;
	lane_right_top.m_count = -1;
	lane_left_top.m_count = -1;
		
	// find right
	for( i=0;i<MAX_LINE_GROUP;i++)
	{
		if( pline_group[i]->m_count <= 0 )
			continue;

		if( pline_group[i]->m_type == LANE_TYPE_TOP_UNKNOWN )
		{
			continue;
		}
		
		if( (pline_group[i]->m_lr == 1) )
		{
			if( lane_right.m_count <= 0 )
			{
//				memcpy(&lane_right, pline_group[i], sizeof(lane_right));
				lane_right = pline_group[i];
			}
			else if( lane_right.m_avg_dis > pline_group[i]->m_avg_dis)
			{
//				memcpy(&lane_right, pline_group[i], sizeof(lane_right));
				lane_right = pline_group[i];
			}				
			pline_group[i]->m_type = LANE_TYPE_SIDE_RIGHT;
		}
	}
	
	// find left
	for( i=0;i<MAX_LINE_GROUP;i++)
	{
		if( pline_group[i]->m_count <= 0 )
			continue;
	
		if( pline_group[i]->m_type == LANE_TYPE_TOP_UNKNOWN )
		{
			continue;
		}
		
		if( (pline_group[i]->m_lr == 0) )
		{
			if( lane_left.m_count <= 0 )
			{
//				memcpy(&lane_left, pline_group[i], sizeof(lane_left));
				lane_left = pline_group[i];
			}
			else if( lane_left.m_avg_dis > pline_group[i]->m_avg_dis)
			{
//				memcpy(&lane_left, pline_group[i], sizeof(lane_left));
				lane_left = pline_group[i];
			}				
			pline_group[i]->m_type = LANE_TYPE_SIDE_LEFT;
		}
		
	}

	// find right top	
	// find left top
	for( i=0;i<MAX_LINE_GROUP;i++)
	{
		int dis0 = -1;
		int dis1 = -1;
		int dis2 = -1;
		int dis3 = -1;
		
		s_line_group * g;
		g = pline_group[i];
		if( g->m_count <= 0 )
		{
			continue;
		}

		if( pline_group[i]->m_type != LANE_TYPE_TOP_UNKNOWN )
		{
			continue;
		}

		// swap top<->bottom
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

		if( lane_left.m_count > 0)
		{
			dis0 = get_points_distance(g->m_point_top, lane_left.m_point_top);
			dis1 = get_points_distance(g->m_point_bot, lane_left.m_point_top);

			if( dis0 > dis1 )
				dis0 = dis1;
		}

		if( lane_right.m_count > 0)
		{
			dis2 = get_points_distance(g->m_point_top, lane_right.m_point_top);
			dis3 = get_points_distance(g->m_point_bot, lane_right.m_point_top);

			if( dis2 > dis3)
				dis2 = dis3;
		}
		
		if( (dis0>0) && (dis2>0) )
		{
			if( dis0 > dis2 )
			{
				g->m_type = LANE_TYPE_TOP_RIGHT;
//				memcpy(&lane_right_top, g, sizeof(lane_right_top));
				lane_right_top = g;
				s_log_info("find right_top case 1 (%d,%d)\n", dis0, dis2);
			}
			else
			{
				g->m_type = LANE_TYPE_TOP_LEFT;
//				memcpy(&lane_left_top, g, sizeof(lane_left_top));
				lane_left_top = g;
				s_log_info("find left_top case 2 (%d,%d)\n", dis0, dis2);
			}
			continue;
			
		}
		
		if( dis0 > 0 )
		{
			if( dis0 < 100 )
			{				
				g->m_type = LANE_TYPE_TOP_LEFT;
//				memcpy(&lane_left_top, g, sizeof(lane_left_top));
				lane_left_top = g;
				s_log_info("find left_top case 3 (%d)\n", dis0);
				continue;
			}
		}
		
		if( dis2 > 0)
		{
			if( dis2 < 100 )
			{				
				g->m_type = LANE_TYPE_TOP_RIGHT;
//				memcpy(&lane_right_top, g, sizeof(lane_right_top));
				lane_right_top = g;
				s_log_info("find right_top case 4 (%d)\n", dis2);
				continue;
			}
		}
		
		// no left and right lane, compare with prevoius 

		// similar with left_top
		if( (g->m_avg_alpha < m_lane_left_top.m_avg_alpha + 10 ) &&
			(g->m_avg_alpha > m_lane_left_top.m_avg_alpha - 10 ) &&
			(g->m_avg_dis < m_lane_left_top.m_avg_dis + 100 ) &&
			(g->m_avg_dis > m_lane_left_top.m_avg_dis - 100) )
		{
			g->m_type = LANE_TYPE_TOP_LEFT;
//			memcpy(&lane_left_top, g, sizeof(lane_left_top));
			lane_left_top = g;
			s_log_info("find left_top case 5\n");
			continue;
		}
		
		if( (g->m_avg_alpha < m_lane_right_top.m_avg_alpha + 10 ) &&
			(g->m_avg_alpha > m_lane_right_top.m_avg_alpha - 10 ) &&
			(g->m_avg_dis < m_lane_right_top.m_avg_dis + 100 ) &&
			(g->m_avg_dis > m_lane_right_top.m_avg_dis - 100) )
		{
			g->m_type = LANE_TYPE_TOP_RIGHT;
//			memcpy(&lane_right_top, g, sizeof(lane_right_top));
			lane_right_top = g;
			s_log_info("find right_top case 6\n");
			continue;
		}				
			
 	}

//	memcpy(&m_lane_right, &lane_right, sizeof(s_line_group));
	m_lane_right = lane_right;

//	memcpy(&m_lane_left, &lane_left, sizeof(s_line_group));
	m_lane_left = lane_left;

//	memcpy(&m_lane_right_top, &lane_right_top, sizeof(s_line_group));
	m_lane_right_top = lane_right_top;

//	memcpy(&m_lane_left_top, &lane_left_top, sizeof(s_line_group));
	m_lane_left_top = lane_left_top;

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
			s_log_info("T%lld--- %s --- count:%d alpha:%d dis:%d bot:(%d, %d) top:(%d,%d)\n",s_timer_t::get_inst()->get_ms(), sz, lg->m_count, lg->m_avg_alpha, lg->m_avg_dis, lg->m_point_bot.m_x, lg->m_point_bot.m_y, lg->m_point_top.m_x, lg->m_point_top.m_y);
		}		
	}

	return 0;
}

int s_drive_player_t::no_lane_showed()
{
	return  ((m_lane_left.m_count <= 0) 
			&& (m_lane_right.m_count <= 0)
			&& (m_lane_left_top.m_count <= 0)
			&& (m_lane_right_top.m_count <= 0));
}

int s_drive_player_t::do_fwd()
{
	int d1;
	int d2;
	int right_up;
	int left_up;
	// see one or multiple lanes
	right_up = -1;
	left_up = -1;

	if( m_lane_right_top.has_lane() )
		right_up = m_lane_right_top.m_avg_dis;
	
	if( m_lane_left_top.has_lane() )
		left_up = m_lane_left_top.m_avg_dis;
	
	if( m_lane_right.has_lane()  &&  m_lane_right_top.has_lane()  )
	{
		d1 = abs(m_lane_right_top.m_point_bot.m_x - m_lane_right.m_point_top.m_x);
		d2 = abs(m_lane_right_top.m_point_top.m_x - m_lane_right.m_point_top.m_x);
		if( d1 < d2 )
		{
			m_next_turn = NEXT_TURN_RIGHT;
			s_log_info("next turn right ...\n");
		}
		else
		{
			m_next_turn = NEXT_TURN_LEFT;
			s_log_info("next turn left ...\n");
		}
	}
	
	if( m_lane_left.has_lane() && m_lane_left_top.has_lane() )
	{
		d1 = abs(m_lane_left_top.m_point_bot.m_x - m_lane_left.m_point_top.m_x);
		d2 = abs(m_lane_left_top.m_point_top.m_x - m_lane_left.m_point_top.m_x);
		if( d1 < d2 )
		{
			m_next_turn = NEXT_TURN_RIGHT;
			s_log_info("next turn right ###\n");
		}
		else
		{
			m_next_turn = NEXT_TURN_LEFT;
			s_log_info("next turn left ###\n");
		}
	}
	
	if( m_next_turn == NEXT_TURN_LEFT )
	{
		if( ((right_up <= 300 ) && (right_up > 0)) )// || (m_lane_right.m_count <= 0) )
		{
			m_next_turn = NEXT_TURN_NONE;
			m_wheel->move_forward_turn(10,25);
			m_run_state = DRV_STATE_TURN_LEFT;
			m_last_time = s_timer_t::get_inst()->get_ms();
			s_log_info("top turn left dis:%d...\n", right_up);
		}
	}
	else if( m_next_turn == NEXT_TURN_RIGHT )
	{
		if( ((left_up <= 300 ) && (left_up > 0)) ) // || (m_lane_left.m_count <= 0) )
		{
			m_next_turn = NEXT_TURN_NONE;
			m_wheel->move_forward_turn(25, 10);
			m_last_time = s_timer_t::get_inst()->get_ms();
			m_run_state = DRV_STATE_TURN_RIGHT;
			s_log_info("top turn right dis:%d...\n", left_up);
		}
	}
	return 0;
}

int s_drive_player_t::do_turn_left()
{
	int64 ms;
	ms = s_timer_t::get_inst()->get_ms();
	ms = ms - m_last_time;
	if( ( ms > TIMEOUT_TURNING_MS) && m_lane_right.has_lane() && (abs(m_lane_right.m_avg_alpha) > 50) )
	{
		m_run_state = DRV_STATE_FWD;
		m_wheel->move_forward(15);
		s_log_info("turn left ---> forward\n");
	}
	return 0;
}

int s_drive_player_t::do_turn_right()
{
	int64 ms;
	ms = s_timer_t::get_inst()->get_ms();
	ms = ms - m_last_time;
	if(  ( ms > TIMEOUT_TURNING_MS) && m_lane_left.has_lane() && (abs(m_lane_left.m_avg_alpha) > 50) )
	{
		m_run_state = DRV_STATE_FWD;
		m_wheel->move_forward(15);
		s_log_info("turn right ---> forward\n");
	}
	return 0;
}

int s_drive_player_t::drive()
{	
 	if( no_lane_showed() )
	{
		m_wheel->move_stop();
		if( m_run_state != DRV_STATE_STOP )
		{	
			m_run_state = DRV_STATE_STOP;			
			s_log_info("drive stopped\n");
		}
		return 0;
	}

	switch (m_run_state )
	{
		case DRV_STATE_FWD:
			do_fwd();
			break;
		case DRV_STATE_TURN_LEFT:
			return do_turn_left();
		case DRV_STATE_TURN_RIGHT:
			return do_turn_right();
	}

	m_run_state = DRV_STATE_FWD;

	if( m_lane_right.has_lane() && m_lane_left.has_lane() )
	{
		int d;
		d = (m_lane_right.m_avg_dis + m_lane_left.m_avg_dis)/2;
		if( m_lane_right.m_avg_dis > d + 50 ) // turn right
		{
			m_wheel->move_forward_turn(15, 10);
			s_log_info("drive turn right ...\n");
		}
		else if( m_lane_right.m_avg_dis < d - 50 ) // turn left
		{
			m_wheel->move_forward_turn(10, 15);
			s_log_info("drive turn left ...\n");
		}
		else
		{
			m_wheel->move_forward(15);
			s_log_info("drive forward ...\n");
		}
	}
	else
	{
		if( m_lane_right.m_avg_dis < 200 )
		{
			m_wheel->move_forward_turn(10, 15);
			s_log_info("drive turn left\n");
		}
		else if( m_lane_right.m_avg_dis > 400 )
		{
			m_wheel->move_forward_turn(15,10);
			s_log_info("drive turn right\n");
		}
		else
		{
			m_wheel->move_forward(15);
			s_log_info("drive forward\n");
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

//	drive();
	SIGNAL(m_loc_signal);

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
	timespec abstime;
	int ret;
	while(!m_quit)
	{
		//	struct timeval current;
		clock_gettime(CLOCK_REALTIME, &abstime);
		abstime.tv_sec += 1;	// timed out 6 seconds
		abstime.tv_nsec += 1000*1000*10;
			
		LOCK_MUTEX(m_loc_mutex);
		ret = WAIT_SIGNAL_TIMEOUT(m_loc_signal, m_loc_mutex, abstime);
		printf("timeout ret:%d\n", ret);
		if( ret != 0 )
		{
		}
		UNLOCK_MUTEX(m_loc_mutex);

		drive();
//		s_sleep_ms(20);
	}
	m_quit = 1;
	return 0;
}

