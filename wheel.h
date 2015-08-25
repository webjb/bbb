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
#define S_MOVE_STATE_REST      6



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
	int move_backward(int speed);
	int move_forward(int speed);
	int move_stop();

    int set_position(int alpha, int z);
	int is_position_lost();

	int start_search();
	int stop_search();

	int start_move();
	int stop_move();

	int is_ball_found();
	int is_ball_reached();
    
private:
	int64 m_last_cmd_time;
	int64 m_last_pos_time;
    int m_alpha;
    int m_z;
    s_j1939_t * m_j1939;
    int m_run_state;
	int m_move_state; // left, right, forward, backward
	s_eye_t * m_eye;
};

#endif
