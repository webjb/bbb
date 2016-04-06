// bob sang robot
#ifndef __S_WHEEL_H__
#define __S_WHEEL_H__

#include "common.h"
#include "servo.h"
#include "timer.h"
#include "j1939.h"
#include "eye.h"

#define S_MOVE_STATE_NOTHING 0
#define S_MOVE_STATE_STOP	1
#define S_MOVE_STATE_LEFT	2
#define S_MOVE_STATE_RIGHT	3
#define S_MOVE_STATE_FORWARD 4
#define S_MOVE_STATE_BACKWARD 5
//#define S_MOVE_STATE_REST      6

#define S_WH_STATE_NOTHING	0
#define S_WH_STATE_STOP	1
#define S_WH_STATE_REST	2
#define S_WH_STATE_FORWARD 3
#define S_WH_STATE_BACKWARD 4
#define S_WH_STATE_AIM_LEFT	5
#define S_WH_STATE_AIM_RIGHT 6
#define S_WH_STATE_AIM_REST_1 7
#define S_WH_STATE_AIM_REST_2 8
#define S_WH_STATE_AIM_SEARCH_DOOR 9


#define S_FUNC_STATE_NOTHING 0
#define S_FUNC_STATE_SEARCH_BALL 1
#define S_FUNC_STATE_MOVE_TO_BALL 2
#define S_FUNC_STATE_AIM_DOOR 3

#define KICK_DISTANCE		70
#define KICK_MAX_ANGLE	120	//140
#define KICK_MIN_ANGLE	60		//40

class s_wheel_t : public s_object_t
{
public:
    s_wheel_t();
    ~s_wheel_t();
private:
public:
   virtual int start();
   virtual int stop();
   virtual int run();

public:
    static s_wheel_t * get_inst();
	
	int turn_left(int speed);
	int turn_right(int speed);
	int turn_left_2way(int speed);
	int turn_right_2way(int speed);
	int move_backward(int speed);
	int move_forward(int speed);
	int move_stop();

    int set_position(int alpha, int z);
	int is_position_lost();

	int start_search(int x);
	int stop_search();

	int start_move();
	int stop_move();

	int start_aim_door();
	int stop_aim_door();

	int is_ball_found();
	int is_ball_reached();
	int is_door_aimed();
	int is_ball_lost();

private:
	int do_search();
	int do_move_to_ball();
	int do_aim_to_door();
    
private:
	int64 m_last_cmd_time;
	int64 m_last_pos_time;
    int m_alpha;
    int m_z;
    s_j1939_t * m_j1939;
    int m_run_state;
	int m_func_state;
	int m_move_state; // left, right, forward, backward
	int m_wh_state;
	s_eye_t * m_eye;

	int m_last_z;
};

#endif
