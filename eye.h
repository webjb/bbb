//bob sang robot
#ifndef __S_EYE_H__
#define __S_EYE_H__

#include "common.h"
#include "servo.h"
#include "timer.h"

#define EYE_MAX_SERVOS  2

typedef struct _s_eye_event_t
{
	int m_type;
	
	int m_timer;
	
	int m_servo_id;
	int m_pos;
	int m_dir;	// 0: -   1: +
	
} s_eye_event_t;

typedef struct _s_eye_command_t
{
    int64 m_start_time_ms;
	int m_servo_id;
	int m_command;
	int m_pos;
	int m_speed;	
	int m_duration;	// ms

	s_eye_event_t m_event;
} s_eye_command_t;

typedef struct _s_ball_postion_t
{
	int m_x;
	int m_y;
	int m_r;

	int m_alpha;
	int m_distance;
} s_ball_postion_t;

typedef struct _s_door_postion_t
{
	int m_x;
	int m_y;
	int m_width;	
} s_door_position_t;

class s_eye_t  : public s_object_t
{
public:
    s_eye_t(int id);
    ~s_eye_t();

public:
	static s_eye_t* get_inst();
	int msg_ball_position(int x, int y, int r);
	int msg_door_location(int x, int y);
	
	int get_door_position(s_door_position_t * pos);
	int get_ball_position(s_ball_postion_t * pos);
	
    int standby(int speed);
    int up(int speed);
    int down(int speed);
    int left(int speed);
    int right(int speed);

	int go_pos_0(int speed);
	int go_pos_1(int speed);

    int search(int speed);
	int stop_search();
    int poweroff(); 

	int get_pos(int * alpha1, int *alph2 );

	
	int is_command_all_done();
	
public:
    virtual int start();
    virtual int stop();
    virtual int run();

public:
	int zoom_in();
	int zoom_out();

	int get_ball(int & x, int & y, int & r);
	int get_door_pos(int * x, int * y, int *width, int * height);

private:
	bool is_event_reached(s_eye_command_t cmd);
	int add_command(int servo_id, int timer_ms, int pos, int speed);
	int add_null_command(int servo_id, int timer_ms);
	int do_command(s_eye_command_t cmd);

	int ask_door_location();
	int ask_ball_location();
	int ask_motion_detect();
	

	int m_id;
	s_servo_t * m_servo[EYE_MAX_SERVOS];    
	list<s_eye_command_t> m_cmd_list;

	int m_is_power_on;

	s_ball_postion_t m_ball;
	s_door_position_t m_door;
};

#endif

