// bob sang

#include "message.h"
#include "app.h"

s_message_t::s_message_t()
{
	m_tcp = new s_tcp_t(6000);
	
	m_tcp_buf = new s_msg_buf_t;
	m_tcp_buf->m_addr = new char[1024*8];
	m_tcp_buf->m_size = 1024*8;
	m_tcp_buf->m_wpos = 0;
	m_tcp_buf->m_rpos = 0;
}

s_message_t::~s_message_t()
{
	if( m_tcp )
		delete m_tcp;

	delete m_tcp_buf->m_addr;
	delete m_tcp_buf;
}
s_message_t * s_message_t::get_inst()
{
	static s_message_t * g_message = NULL;
	if ( !g_message )
		g_message = new s_message_t();

	return g_message;
}

int s_message_t::start()
{
	m_tcp->set_callback(tcp_recv_callback, (void*)this);
	m_tcp->start();
	return 0;
}

int s_message_t::stop()
{
	m_tcp->stop();
	return 0;
}

int s_message_t::tcp_recv_callback(void * parent,void * para)
{
	s_message_t * msg;
	s_buffer_t * buf;
	msg = (s_message_t*)parent;
	buf = (s_buffer_t*)para;

//RINT_INFO("tcp_recv_callback len:%d %s\n", buf->m_length, buf->m_buffer);
	memcpy(msg->m_tcp_buf->m_addr + msg->m_tcp_buf->m_wpos, buf->m_buffer, buf->m_length);
	msg->m_tcp_buf->m_wpos += buf->m_length;

	msg->tcp_parser();
	return 0;	
}
#if 0
int s_message_t::tcp_parser()
{
	s_command_header_t *header;
	s_app_t * s_app = s_app_t::get_app();
	if( m_tcp_buf->m_wpos < (int)sizeof(s_command_header_t) )
	{
		return 0;
	}
	header = (s_command_header_t*)m_tcp_buf->m_addr;
	if( header->m_payload_length + (int)sizeof(s_command_header_t) > m_tcp_buf->m_wpos )
	{
		return 0;
	}
	
	switch ( header->m_cmd )
	{
		case S_CMD_ARM_KICK:
			PRINT_INFO("cmd: ARM_KICK\n");
			s_app->arm_kick();
			break;
		case S_CMD_ARM_STANDBY:
			PRINT_INFO("cmd: ARM_STANDBY\n");
			s_app->arm_standby();
			break;
		case S_CMD_ARM_POWER_OFF:
			PRINT_INFO("cmd: ARM_POWER_OFF\n");
			s_app->arm_power_off();
			break;
	}

	if( m_tcp_buf->m_wpos <= header->m_payload_length + (int) sizeof(s_command_header_t) )
	{		
		m_tcp_buf->m_wpos = 0;
	}
	else
	{
		memcpy(m_tcp_buf->m_addr, m_tcp_buf->m_addr + m_tcp_buf->m_wpos, m_tcp_buf->m_wpos - (header->m_payload_length + sizeof(s_command_header_t)));
		m_tcp_buf->m_wpos -= header->m_payload_length + (int)sizeof(s_command_header_t);		
	}
	
	return 0;
}
#endif

int s_message_t::tcp_parser()
{
	s_app_t * s_app = s_app_t::get_app();
	int cmd = 0;
//	PRINT_INFO("tcp_parser %s\n", m_tcp_buf->m_addr);
	if( strstr(m_tcp_buf->m_addr, "kick") != 0 )
		cmd = S_CMD_ARM_KICK;
	else if( strstr(m_tcp_buf->m_addr, "standby") != 0)
		cmd = S_CMD_ARM_STANDBY;
	else if( strstr(m_tcp_buf->m_addr, "power off") != 0)
		cmd = S_CMD_ARM_POWER_OFF;
	else if( strstr(m_tcp_buf->m_addr, "position") != 0)
		cmd = S_CMD_CAM_BALL_LOCATION;
	else if( strstr(m_tcp_buf->m_addr, "doorloc") != 0)
		cmd = S_CMD_CAM_DOOR_LOCATION;
	else if( strstr(m_tcp_buf->m_addr, "forward") != 0)
		cmd = S_CMD_WHEEL_FORWARD;
	else if( strstr(m_tcp_buf->m_addr, "backward") != 0)
		cmd = S_CMD_WHEEL_BACKWARD;
	else if( strstr(m_tcp_buf->m_addr, "stop") != 0)
		cmd = S_CMD_WHEEL_STOP;
	else if( strstr(m_tcp_buf->m_addr, "search ball" ) != 0 )
		cmd = S_CMD_EYE_START_SEARCH;
	else if( strstr(m_tcp_buf->m_addr, "stop search" ) != 0 )
		cmd = S_CMD_EYE_STOP_SEARCH;
	else if( strstr(m_tcp_buf->m_addr, "shoot ball" ) != 0 )
		cmd = S_CMD_WHEEL_GO_KICK_BALL;
	
	switch ( cmd )
	{
		case S_CMD_ARM_KICK:
			PRINT_INFO("cmd: ARM_KICK\n");
			s_app->arm_kick();
			break;
		case S_CMD_ARM_STANDBY:
			PRINT_INFO("cmd: ARM_STANDBY\n");
			s_app->arm_standby();
			break;
		case S_CMD_ARM_POWER_OFF:
			PRINT_INFO("cmd: ARM_POWER_OFF\n");
			s_app->arm_power_off();
			break;
		case S_CMD_CAM_BALL_LOCATION:
			{
				int x;
				int y;
				int r;
				char * pbuf = strstr(m_tcp_buf->m_addr, "position");
				pbuf += 9;
				x = atoi(pbuf);
				pbuf = strstr(m_tcp_buf->m_addr, ";");
				pbuf ++;
				y = atoi(pbuf);
				pbuf = strstr(pbuf, ";");
				pbuf ++;
				r = atoi(pbuf);
		
//				PRINT_INFO("cmd: CAM_POSITION %s x:%d y:%d r:%d\n", m_tcp_buf->m_addr, x, y, r);
				s_app->eye_position(x, y, r);
			}
			break;
		case S_CMD_CAM_DOOR_LOCATION:
			{
				int x;
				int y;
				char * pbuf = strstr(m_tcp_buf->m_addr, "doorloc");
				pbuf += 8;
				x = atoi(pbuf);
				pbuf = strstr(m_tcp_buf->m_addr, ";");
				pbuf ++;
				y = atoi(pbuf);
				s_app->door_location(x, y);
			}
			break;
		case S_CMD_WHEEL_FORWARD:
			s_app->m_wheel->move_forward(10);
			break;
		case S_CMD_WHEEL_BACKWARD:
			s_app->m_wheel->move_backward(10);
			break;
		case S_CMD_WHEEL_STOP:
			s_app->m_wheel->move_stop();
			break;
		case S_CMD_WHEEL_GO_KICK_BALL:
//			s_app->wheel_start_move();
			break;
		case S_CMD_EYE_START_SEARCH:
//			s_app->eye_start_search();
			break;
		case S_CMD_EYE_STOP_SEARCH:
//			s_app->eye_stop_search();
			break;
	}
	// just clear all, need to improvement
	m_tcp_buf->m_wpos = 0;
	m_tcp_buf->m_addr[0] = 0;
	return 0;
}
