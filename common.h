// bob sang robot

#ifndef __S_COMMON_H__
#define __S_COMMON_H__

#include "def.h"

#include "object.h"
#include "timer.h"

#define S_STATE_NONE	0
#define S_STATE_RUN		1
#define S_STATE_STOP	2

#if 0
#define S_RUN_STATE_NOTHING       0
#define S_RUN_STATE_MOVING			2
#define S_RUN_STATE_SEARCH			3
#define S_RUN_STATE_STOP			4
#define S_RUN_STATE_START_EYE_SEARCH	5
#define S_RUN_STATE_STOP_EYE_SEARCH 6
#define S_RUN_STATE_START_WHEEL_SEARCH 7
#define S_RUN_STATE_STOP_WHEEL_SEARCH 8
#define S_RUN_STATE_START_WHEEL_MOVE   9
#define S_RUN_STATE_STOP_WHEEL_MOVE	10
#define S_RUN_STATE_START_ARM_KICK    11
#define S_RUN_STATE_STOP_ARM_KICK		12
#define S_RUN_STATE_WAIT_ARM_RL		13
#endif

#ifndef int64
typedef long long int64;
#endif

#define PRINT_INFO printf

int64 get_current_time();	// us

#define SYSTEM_CALL(x) \
{ \
	system(x); \
}

#define PRINT_INFO printf
#define PRINT_ERROR printf

#define FOUND_BALL(x, y) ( (x!= 0 ) || ( y != 0) )

#define LINE_BALL(alpha) (  (alpha >= 80)  && (alpha <= 100) )

#define NEAR_BALL(alpha, z) \
	( (alpha < 140 && alpha > 40 ) && (z <= KICK_DISTANCE && z > 1 ) )

#define GET_MS() (s_timer_t::get_inst()->get_ms())



typedef int (*s_callback_t)(void* userdata, void * parg);

typedef struct s_buffer_t_
{
    char * m_buffer;
    int m_length;
} s_buffer_t;


enum
{
	S_CMD_MOVE_TO,
	S_CMD_POWER_OFF,
	S_CMD_PAUSE,
	S_CMD_WAIT,
	
};

enum
{
	S_EVENT_NOTHING,
	S_EVENT_TIMER,
	S_EVENT_SERVO_POS_REACHED,	
};

#endif

