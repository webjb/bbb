// bob sang robot

#ifndef __S_SERVO_H__
#define __S_SERVO_H__

#include "common.h"
#include "object.h"

class s_servo_t : public s_object_t
{
public:
	s_servo_t(string name, int id, int standby_pos, int min_pos, int max_pos);
	virtual ~s_servo_t();
public:

	int power_on();
	int power_off();
	int move_to(int pos, int speed); // pos: 0-100 speed: 1-100
	int get_pos();

    int pwm_set_run(int power);
    int pwm_set_duty(int duty);
    int pwm_set_period(int period);
    int pwm_set_polarity(int polarity);
    
    int standby(int speed=10);
    int is_standby_done();

	int stop_move();
        
public:
    virtual int start();
    virtual int stop();
	virtual int run();
private:
	int m_id;
	int m_period;
	int m_duty;
	int m_speed;
	int m_cur_pos;
	int m_next_pos;
	int m_state;
	int64 m_last_time;

	char m_name[MAX_NAME_LENGTH];	

    int m_duty_steps;
    int m_cur_duty;
    int m_next_duty;

    int m_standby_pos;
    int m_min_pos;
    int m_max_pos;

    int m_is_power_on;
};



#endif
