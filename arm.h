// bob sang

#ifndef __S_ARM_H__
#define __S_ARM_H__

#include "common.h"
#include "servo.h"
#include "timer.h"

#define MAX_SERVOS  6

typedef struct _s_arm_event_t
{
	int m_type;
	
	int m_timer;
	
	int m_servo_id;
	int m_pos;
	int m_dir;	// 0: -   1: +
	
} s_arm_event_t;

typedef struct _s_arm_command_t
{
    int64 m_start_time_ms;
	int m_servo_id;
	int m_command;
	int m_pos;
	int m_speed;	
	int m_duration;	// ms

	s_arm_event_t m_event;
} s_arm_command_t;

class s_arm_t : public s_object_t
{
public:
    s_arm_t(int id);
    ~s_arm_t();

public:
	static s_arm_t* get_inst();
    int standby(int speed);
    int up(int speed);
    int down(int speed);
	int left(int speed);
	int right(int speed);

	int go_pos_front(int speed);
	int go_pos_right(int speed);
	int go_pos_3(int speed);
	
	int right_low(int speed);
    int kick(int speed);
    int poweroff();
	int is_command_all_done();
	
public:
    virtual int start();
    virtual int stop();
    virtual int run();
private:
    bool is_event_reached(s_arm_command_t cmd);
    int add_command(int servo_id, int timer_ms, int pos, int speed);
    int do_command(s_arm_command_t cmd);
    int add_power_off_cmd(int servo_id, int timer_ms);
    int m_id;
    s_servo_t * m_servo[MAX_SERVOS];    
	list<s_arm_command_t> m_cmd_list;

    int m_is_power_on;
};

#endif
