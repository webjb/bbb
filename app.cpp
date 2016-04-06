//
#include "app.h"
#include "message.h"

#include <math.h>

s_app_t::s_app_t()
{
	m_msg = s_message_t::get_inst();

	m_wheel = s_wheel_t::get_inst();
	m_eye = s_eye_t::get_inst();
	m_arm = s_arm_t::get_inst();
	m_msg_player = s_massage_player_t::get_inst();
	
	m_soccer_player = s_soccer_player_t::get_inst();
		
    CREATE_THREAD(m_keyboard_thread, thread_keyboard_entry, 0x8000, this, 1);         // Reserve 32KB stack

	m_last_ms = 0;
	m_run_state = S_RUN_STATE_NOTHING;

	m_exit = 0;
	
	m_msg->start();
	m_wheel->start();
}

s_app_t::~s_app_t()
{
	m_msg->stop();

}

s_app_t* s_app_t::get_app()
{
	static s_app_t * g_app = NULL;
	if( !g_app )
		g_app = new s_app_t();

	return g_app;
}

int s_app_t::start()
{
	if( is_started() )
		return 0;
	s_object_t::start();
	m_soccer_player->start();

	m_run_state = 0;
	return 0;
}

int s_app_t::stop()
{
	PRINT_INFO("app stop\n");
	if( !is_started() )
		return 0;
	s_object_t::stop();

	m_soccer_player->stop();
	return 0;
}

int s_app_t::arm_kick()
{
	m_arm->kick(10);
	return 0;
}

int s_app_t::arm_standby()
{
	m_arm->standby(10);

	return 0;
}

int s_app_t::arm_power_off()
{
	m_arm->poweroff();
	return 0;
}

int s_app_t::eye_position(int x,int y, int r)
{
	return m_eye->msg_ball_position(x,y,r);
}

int s_app_t::door_location(int x,int y)
{
	return m_eye->msg_door_location(x,y);
}

int s_app_t::run_keyboard()
{
	char ch;
	int speed = 10;
	int wheel_speed = 20;
	
	while(1)
	{
		ch = getchar();
		PRINT_INFO("ch=%c\n", ch);
		switch(ch)
		{
			case 'a':
				start();
				break;
			case 'A':
				stop();
				break;					
			case '0':
				m_arm->poweroff();
				break;
			case '1':
//				m_arm->right_low(speed);
				m_eye->start();
				m_eye->standby(10);

				break;
			case '2':
				m_arm->standby(speed);
				break;
			case '3':
				m_arm->kick(speed);
				break;
			case '4':
				m_arm->go_pos_front(speed);
				break;
			case '5':
				m_eye->go_pos_0(speed);
				break;				
			case '6':
				m_eye->go_pos_1(speed);
				break;
			case '+':
				wheel_speed += 10;
				if( wheel_speed > 100 )
					wheel_speed = 100;
				break;
			case '-':
				wheel_speed -= 10;
				if( wheel_speed < 10 )
					wheel_speed = 10;
				break;
			case 'l':				
				m_wheel->turn_left_2way(wheel_speed);
				break;
			case 'r':
				m_wheel->turn_right_2way(wheel_speed);
				break;
			case 'L':				
				m_wheel->turn_left(wheel_speed);
				break;
			case 'R':
				m_wheel->turn_right(wheel_speed);
				break;
			case 's':
				m_wheel->move_stop();
				break;			
			case 'f':
				PRINT_INFO("FORWARD: speed:%d\n", wheel_speed);
				m_wheel->move_forward(wheel_speed);
				break;
			case 'b':
				PRINT_INFO("BACKWARD: speed:%d\n", wheel_speed);
				m_wheel->move_backward(wheel_speed);
				break;
			case 'e':
				m_eye->standby(speed);
				break;
			case 'E':
				m_eye->search(speed);
				break;
			case 'm':
				m_msg_player->start();
				break;
			case 'q':				
				m_eye->stop();
				m_msg_player->stop();
				break;				
		}
		if( ch == 'q')
			break;
	}
	m_exit = 1;
	if( m_quit) 
		m_exit = 2;
	return 0;
}

int s_app_t::is_quit()
{
	return (m_exit == 2);
}

void * s_app_t::thread_keyboard_entry(void * pval)
{
   	s_app_t *papp = (s_app_t*)pval;

	papp->run_keyboard();
	return NULL;
}

int s_app_t::run()
{
//	int64 time_ms;
	while(!this->m_quit)
	{
		if( m_exit == 1)
			break;
	 	switch( m_run_state )
		{
	 	}
		usleep(1000*50);
	}
	m_quit = 1;
	PRINT_INFO("s_app_t::run EXIT quit:%d\n", this->m_quit);
	if( m_exit == 1 )
	{
		m_eye->stop();
		stop();
		m_exit = 2;
	}

	return 0;
}

