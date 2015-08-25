// bob sang

#include "eye.h"

#define TIME_STAMP() (s_timer_t::get_inst()->get_str().c_str())

#define EYE_POS_LEFT_RIGHT_MIN	10
#define EYE_POS_LEFT_RIGHT_MAX 90

#define EYE_POS_UP_DOWN_MIN		20
#define EYE_POS_UP_DOWN_MAX		90


#define BALL_X	 1100
#define BALL_Y    420

#define IS_TARGET_LOC(x, y) \
	( ( (x < BALL_X + 100) && (x > BALL_X -100 ) ) \
	&& ( (y < BALL_Y + 100) && (y > BALL_Y - 100) ) \
	)

#define IS_LEFT_LOC(x, y) \
	( ( (x < BALL_X + 100) && (x > BALL_X -100 ) ) \
	&& ( (y < BALL_Y + 100) && (y > BALL_Y - 100) ) \
	)
	
#define IS_RIGHT_LOC(x, y) \
	( ( (x < BALL_X + 100) && (x > BALL_X -100 ) ) \
	&& ( (y < BALL_Y + 100) && (y > BALL_Y - 100) ) \
	)

#define PI 3.141592653

void convert_xy(int x0, int y0, int * xout, int * yout)
{
	*yout = BALL_X - x0;
	*xout = BALL_Y - y0;

}

int get_angle(int x, int y, int * angle, int * distance)
{
	double t;
	double dx;
	double dy;
	if( x == 0)
	{
//		PRINT_INFO("x can't be 0\n");
		if( y >= 0 )
		{
			*distance = y;
			*angle = 90;
		}
		else
		{
			*distance = -1*y;
			*angle = 270;
		}
		return 0;
	}
	dx = (double)abs(x);
	dy = (double)abs(y);
	t = dy / dx;
	t = atan(t);
	t *= 180.0/PI;

	if( x > 0 && y >= 0 )
	{
	}
	else if( (x < 0) && (y >= 0 ) )
	{
		t = 180.0 - t;
	}
	else if( (x < 0) && ( y <= 0 ) )
	{
		t = t + 180.0;
	}
	else
		t = 360.0 - t;
		
	*angle = (int)t;
	*distance = (int)sqrt(dx*dx + dy*dy);
	return 0;
}


s_eye_t::s_eye_t(int id)
{
	int i;
	for( i=0; i<EYE_MAX_SERVOS; i++)
	{
		m_servo[i] = NULL;
	}
	m_servo[0] = new s_servo_t("neck", 4, 45, EYE_POS_LEFT_RIGHT_MIN, EYE_POS_LEFT_RIGHT_MAX);
	m_servo[1] = new s_servo_t("head", 5, 30, EYE_POS_UP_DOWN_MIN, EYE_POS_UP_DOWN_MAX);

	m_id = id;
	m_cmd_list.clear();

	m_is_power_on = 0;
}

s_eye_t::~s_eye_t()
{
	int i;
	for(i=0; i<EYE_MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			delete m_servo[i];
	}
}

s_eye_t* s_eye_t::get_inst()
{
	static s_eye_t * g_eye = NULL;
	if( !g_eye )
		g_eye = new s_eye_t(0);

	return g_eye;
}

int s_eye_t::add_command(int servo_id, int timer_ms, int pos, int speed)
{
	s_eye_command_t cmd;
	int64 time_ms;

	time_ms = s_timer_t::get_inst()->get_ms();
	PRINT_INFO("%s time:%lld pos:%d speed:%d\n", TIME_STAMP(), time_ms, pos, speed);
	
	memset(&cmd, 0, sizeof(s_eye_command_t));
	cmd.m_start_time_ms = time_ms + timer_ms;
	cmd.m_servo_id = servo_id;
	cmd.m_command = S_CMD_MOVE_TO;
	cmd.m_speed = speed;
	cmd.m_pos = pos;
//	cmd.m_duration = 1000;
//	cmd.m_event.m_type = S_EVENT_NOTHING;
	m_cmd_list.push_back(cmd);

	return 0;
}

int s_eye_t::add_null_command(int servo_id, int timer_ms)
{
	s_eye_command_t cmd;
	int64 time_ms;

	time_ms = s_timer_t::get_inst()->get_ms();

	memset(&cmd, 0, sizeof(s_eye_command_t));
	cmd.m_start_time_ms = time_ms + timer_ms;
	cmd.m_servo_id = servo_id;
	cmd.m_command = S_CMD_WAIT;
	m_cmd_list.push_back(cmd);

	return 0;
}


int s_eye_t::standby(int speed)
{
	int i;
	PRINT_INFO("STANDBY %d\n", speed);
	for(i=0; i<EYE_MAX_SERVOS; i++)
	{
		m_servo[i]->standby(speed);
		usleep(1000*2000);
	}
	
	return 0;
}

#if 0
int s_eye_t::search(int speed)
{
	int time;
	int i;
	int j;
	int step;
	int count;
	int up_count;
	int up_step;
	int up_pos0;
	int up_pos;
	PRINT_INFO("KICK %d\n", speed);

	time = 0;
	count = 10;
	up_count = 5;
	
	up_pos0 = 25;
	add_command(1, time, up_pos0, 100);
	time += 1000;
	add_command(0, time, EYE_POS_LEFT_RIGHT_MIN, 100);
	time += 2000;
	step = (EYE_POS_LEFT_RIGHT_MAX - EYE_POS_LEFT_RIGHT_MIN) / count;

	up_step = (35 - up_pos0)/up_count;
	
	for( i=0;i<count;i++)
	{
		add_command(0, time, EYE_POS_LEFT_RIGHT_MIN + step * i, 10);
		for( j=0;j<up_count;j++)
		{
			if( (i%2) == 0 )				
				up_pos = up_pos0 + up_step*j;
			else
				up_pos = up_pos0 + up_count*up_step - j*up_step;
			add_command(1, time, up_pos, 20);
			time += 1000;
		}
	}
	
	return 0;
}
#endif

int s_eye_t::search(int speed)
{
	int time;
	int i;
	int step;
	int count;
	time = 0;
	count = 10;
	add_command(1, time, 30, 100);
	time += 1000;
	add_command(0, time, EYE_POS_LEFT_RIGHT_MIN, 100);
	time += 2000;
	step = (EYE_POS_LEFT_RIGHT_MAX - EYE_POS_LEFT_RIGHT_MIN) / count;
	for( i=0;i<count;i++)
	{
		add_command(0, time, EYE_POS_LEFT_RIGHT_MIN + step * i, 20);
		time += 200;
	}
	
	return 0;
}


int s_eye_t::stop_search()
{
	int i;
	for( i=0; i<EYE_MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			m_servo[i]->stop_move();
	}
	m_cmd_list.clear();
	return 0;
}

int s_eye_t::get_pos(int * alpha1, int * alpha2)
{
	int pos0;
	int pos1;
	pos0 = m_servo[0]->get_pos();
	pos1 = m_servo[1]->get_pos();

	if( pos0 > EYE_POS_LEFT_RIGHT_MAX )
		pos0 = EYE_POS_LEFT_RIGHT_MAX;
	if( pos0 < EYE_POS_LEFT_RIGHT_MIN)
		pos0 = EYE_POS_LEFT_RIGHT_MIN;

	if( pos1 > EYE_POS_UP_DOWN_MAX )
		pos1 = EYE_POS_UP_DOWN_MAX;
	if( pos1 < EYE_POS_UP_DOWN_MIN)
		pos1 = EYE_POS_UP_DOWN_MIN;
	
	*alpha1 = (90 - pos0) * 2;
	
	*alpha2 = (90 - pos1) * 2;

	return 0;
}

int s_eye_t::go_pos_0(int speed)
{
	PRINT_INFO("EYE go pos 0\n");
	add_command(0, 0000, 45, 10);
	add_command(1, 0000, 45, 10);
	add_null_command(1, 3000);
	return 0;
}

int s_eye_t::go_pos_1(int speed)
{
	PRINT_INFO("EYE go pos 1\n");
	add_command(0, 0000, 45, 10);
	add_command(1, 0000, 30, 10);
	add_null_command(1, 3000);
	return 0;
}

int s_eye_t::up(int speed)
{
	PRINT_INFO("EYE up %d\n", speed);
	add_command(1, 4000, 20, speed);
	
	return 0;
}

int s_eye_t::down(int speed)
{
	PRINT_INFO("EYE down %d\n", speed);
	add_command(1, 3000, 100, speed);
	return 0;
}

int s_eye_t::left(int speed)
{
	PRINT_INFO("left %d\n", speed);
	return 0;
}

int s_eye_t::right(int speed)
{
	PRINT_INFO("right %d\n", speed);
	return 0;
}

int s_eye_t::poweroff()
{
	int i;
	PRINT_INFO("poweroff\n");
	
	for(i=0; i<EYE_MAX_SERVOS; i++)
	{
		m_servo[i]->power_off();
	}
	
	return 0;
}

int s_eye_t::ask_door_location()
{
	return 0;
}

int s_eye_t::ask_ball_location()
{
	return 0;
}

int s_eye_t::ask_motion_detect()
{
	return 0;	
}


int s_eye_t::start()
{
	int i;
	s_object_t::start();
	for( i=0; i<EYE_MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			m_servo[i]->start();
	}
	
	return 0;
}

int s_eye_t::stop()
{
	int i;
	for( i=0; i<EYE_MAX_SERVOS; i++)
	{
		if( m_servo[i] )
			m_servo[i]->stop();
	}
	s_object_t::stop();
	m_cmd_list.clear();
	
	return 0;
}

int s_eye_t::msg_ball_position(int x,int y,int r)
{
	m_ball.m_x = x;
	m_ball.m_y = y;
	m_ball.m_r = r;
	
	return 0;
}

int s_eye_t::msg_door_location(int x,int y)
{
	m_door.m_x = x;
	m_door.m_y = y;
	m_door.m_r = 0;
	
	return 0;
}

int s_eye_t::get_door_position(s_door_position_t * pos)
{
	return 0;
}

int s_eye_t::get_ball_position(s_ball_postion_t * ball)
{
	int xout;
	int yout;
	int alpha;
	int z;
	int r;
	int x;
	int y;

	x = m_ball.m_x;
	y = m_ball.m_y;
	r = m_ball.m_r;

	ball->m_x = x;
	ball->m_y = y;
	ball->m_r = r;
	
	ball->m_alpha = 0;
	ball->m_distance = -1;
	
	if( r < 10 )
	{
//		PRINT_INFO("radius is too small x=%d y=%d r=%d, might be noise\n", x, y, r);
		return 0;
	}
	convert_xy(x,y, &xout, &yout);
	get_angle(xout,yout, &alpha, &z);

	ball->m_alpha = alpha;
	ball->m_distance = z;

	return 0;
}

bool s_eye_t::is_event_reached(s_eye_command_t cmd)
{
	int64 time_ms;

	time_ms = s_timer_t::get_inst()->get_ms();

//	PRINT_INFO("is %lld %lld\n", time_ms, cmd.m_start_time_ms);
	if( cmd.m_start_time_ms == -1 )
		return true;

	if( time_ms >= cmd.m_start_time_ms )
		return true;
	else
		return false;

	if( cmd.m_event.m_type == S_EVENT_NOTHING)
		return true;

	if(cmd.m_event.m_type == S_EVENT_TIMER )
	{
//		int cur_time;
//		cur_time = get_current_time();
//		if( cur_time > cmd.m_event.m_timer )
			return true;		
	}

	if( cmd.m_event.m_type == S_EVENT_SERVO_POS_REACHED )
	{
		int id;
		int pos;
		id = cmd.m_event.m_servo_id;
		pos = m_servo[id]->get_pos();

		if( cmd.m_event.m_dir == 1 )
		{
			if( pos >= cmd.m_event.m_pos )
				return true;
		}
		else 
		{
			if( pos <= cmd.m_event.m_pos )
				return true;
		}
	}
	return false;
}

int s_eye_t::is_command_all_done()
{
	if( m_cmd_list.empty() )
		return 1;
	else
		return 0;
}

int s_eye_t::do_command(s_eye_command_t cmd)
{
	PRINT_INFO("\n%s do_command\n", TIME_STAMP());
	switch(cmd.m_command)
	{
		case S_CMD_MOVE_TO:		
			m_servo[cmd.m_servo_id]->move_to(cmd.m_pos, cmd.m_speed);
			break;
	}
	return 0;
}


int s_eye_t::run()
{
	PRINT_INFO("E s_eye_t::run()\n");
	while(!m_quit)
	{
		list<s_eye_command_t>::iterator it;
		if( m_cmd_list.empty()  )
		{				
			goto __eye_loop;
		}

		for(it = m_cmd_list.begin(); it != m_cmd_list.end(); it ++)
		{
			if( is_event_reached(*it) ) 
			{
				do_command(*it);
				m_cmd_list.erase(it);
				break;
			}
		}
		usleep(1000*20);
		continue;
__eye_loop:
		usleep(1000*100);
	}
	m_quit = 1;
	PRINT_INFO("X s_eye_t::run()\n");
	return 0;
}

