//bob sang robot
#ifndef __MASSAGEPLAYER_H__
#define __MASSAGEPLAYER_H__

#include "common.h"
#include "object.h"

#include "eye.h"
#include "arm.h"
#include "wheel.h"
#include "gpio.h"

typedef struct _s_msg_event_t
{
	int m_type;
	
	int m_timer;
	
	int m_servo_id;
	int m_pos;
	int m_dir;	// 0: -   1: +
	
} s_msg_event_t;

typedef struct _s_msg_command_t
{
    int64 m_start_time_ms;
	int m_servo_id;
	int m_command;
	int m_pos;
	int m_speed;	
	int m_duration;	// ms

	s_msg_event_t m_event;
} s_msg_command_t;


class s_massage_player_t : public s_object_t
{
public:
	s_massage_player_t();
	~s_massage_player_t();
public:	
    static s_massage_player_t* get_inst();
    virtual int start();
    virtual int stop();
    virtual int run();	

	int play();

private:
	
    int m_run_state;
	int m_exit;
    bool is_event_reached(s_msg_command_t cmd);
    int add_command(int servo_id, int timer_ms, int pos, int speed);
    int do_command(s_msg_command_t cmd);

	s_servo_t * m_servo[MAX_SERVOS];	
	list<s_msg_command_t> m_cmd_list;

	s_gpio_t * m_gpio[2];


};

#endif

