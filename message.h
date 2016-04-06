// bob sang
#ifndef __S_MESSAGE_H__
#define __S_MESSAGE_H__

#include "common.h"
#include "tcp.h"

enum
{
    S_CMD_ARM_KICK = 100,
    S_CMD_ARM_STANDBY,
    S_CMD_ARM_POWER_OFF,
    S_CMD_CAM_BALL_LOCATION,
    S_CMD_CAM_DOOR_LOCATION,
    S_CMD_WHEEL_FORWARD,
    S_CMD_WHEEL_BACKWARD,
    S_CMD_WHEEL_STOP,
    S_CMD_WHEEL_GO_KICK_BALL,
    S_CMD_EYE_START_SEARCH,
    S_CMD_EYE_STOP_SEARCH,

	S_CMD_HOST_ASK_DOOR_LOCATION,
	S_CMD_HOST_ASK_BALL_LOCATION,
	S_CMD_HOST_ASK_MOTION_DETECT,

	S_CMD_LANE_LOCTION,
};

typedef struct s_command_header_t_
{
    char m_prem[4];     // SCM:
    int m_cmd;         // 0, 1
    int m_payload_length;
} s_command_header_t;

typedef struct s_msg_buf_t_
{
    char * m_addr;
    int m_size;
    int m_wpos;
    int m_rpos;
} s_msg_buf_t;

class s_message_t 
{
public:
    s_message_t();
    ~s_message_t();
public:
    static s_message_t * get_inst();
    int tcp_parser();
    int can_parser();
    static int tcp_recv_callback(void * parent, void * para);

    int start();
    int stop();

	int send();
	
public:
    s_tcp_t * m_tcp;
    s_msg_buf_t * m_tcp_buf;
};
#endif
