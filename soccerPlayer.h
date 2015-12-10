//bob sang robot
#ifndef __SOCCERPLAYER_H__
#define __SOCCERPLAYER_H__

#include "common.h"
#include "object.h"

#include "eye.h"
#include "arm.h"
#include "wheel.h"

class s_soccer_player_t : public s_object_t
{
public:
	s_soccer_player_t();
	~s_soccer_player_t();
public:	
    static s_soccer_player_t* get_inst();
    virtual int start();
    virtual int stop();
    virtual int run();
	
    int eye_watch();
    int eye_standby();
    int eye_power_off();
	
	int eye_start_search();
	int eye_stop_search();

	int wheel_start_search();
	int wheel_stop_search();	
	int wheel_start_move();
	int wheel_stop_move();

	int arm_start_kick();
	int arm_stop_kick();

	int kick_ball();

private:
	
    int m_run_state;
	int m_exit;
    
    s_arm_t * m_arm;
    s_eye_t * m_eye;
    s_wheel_t * m_wheel;	
};

#endif

