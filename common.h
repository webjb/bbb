// bob sang robot

#ifndef __S_COMMON_H__
#define __S_COMMON_H__

#include "def.h"

#include "object.h"
#include "timer.h"

#define S_STATE_NONE	0
#define S_STATE_RUN		1
#define S_STATE_STOP	2

enum
{
	S_RUN_STATE_NOTHING = 0,

	S_RUN_STATE_EYE_SEARCH_DOOR,
	S_RUN_STATE_EYE_SEARCH_DOOR_START,
	S_RUN_STATE_EYE_SEARCH_DOOR_STOP,
	
	S_RUN_STATE_EYE_SEARCH_BALL,
	S_RUN_STATE_EYE_SEARCH_BALL_START,
	S_RUN_STATE_EYE_SEARCH_BALL_STOP,
	
	S_RUN_STATE_EYE_SEARCH_FRAME,

	S_RUN_STATE_EYE_RETURN_ORIG,

	S_RUN_STATE_WHEEL_TURN,
	S_RUN_STATE_WHEEL_MOVE_TO_BALL,

	S_RUN_STATE_ARM_KICK,
	
};


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

