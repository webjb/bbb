// bob sang

#ifndef __S_APP_H__
#define __S_APP_H__

#include "object.h"
#include "arm.h"
#include "eye.h"
#include "wheel.h"
#include "soccerPlayer.h"
#include "movingPlayer.h"
#include "message.h"

int get_angle(int x, int y, int * angle, int * distance);

enum
{
	S_FUNC_WAIT = 0,
	S_FUNC_KICK_BALL = 1,
	S_FUNC_KICK_MOVING_BALL,
	S_FUNC_DANCE,
	S_FUNC_MUSIC,
};

enum
{
	S_FUNC_KB_SEARCH_BALL,
	S_FUNC_KB_SEARCH_DOOR,
	S_FUNC_KB_MOVE_TO_BALL,
	S_FUNC_KB_KICK_BALL,

	S_FUNC_KBM_SEARCH_BALL,

	S_FUNC_DAN_LISTEN,
	
};

class s_app_t : public s_object_t
{
public:
    s_app_t();
    ~s_app_t();

public:    
    static s_app_t* get_app();
    virtual int start();
    virtual int stop();

    int start_moving_player();
    int stop_moving_player();
    virtual int run();
public:
    int arm_kick();
    int arm_standby();
    int arm_power_off();

    int eye_position(int x, int y, int r);
	int door_location(int x, int y);
    int is_quit();
public:
	s_wheel_t * m_wheel;
    
private:
	static void * thread_keyboard_entry(void *);
	int run_keyboard();
private:

    int m_run_state;
	
	s_message_t * m_msg;
    
    s_arm_t * m_arm;
    s_eye_t * m_eye;
	s_soccer_player_t * m_soccer_player;
    s_moving_player_t * m_moving_player;
	
    thread_t m_keyboard_thread;

	int64	m_last_ms;
	int m_exit;

	
	int m_alpha1;
	int m_alpha2;
};

#endif

