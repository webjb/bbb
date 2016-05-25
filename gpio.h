// bob sang robot

#ifndef __S_GPIO_H__
#define __S_GPIO_H__

#include "common.h"
#include "object.h"

class s_gpio_t : public s_object_t
{
public:
	s_gpio_t(int id, int bit);
	virtual ~s_gpio_t();
public:

	int power_on();
	int power_off();

    int set_low();
	int set_high();
public:
    virtual int start();
    virtual int stop();
	virtual int run();
private:
	int m_id;
	int m_bit;

	int m_status;
	
	int64 m_last_time;

	char m_name[MAX_NAME_LENGTH];	

    int m_is_power_on;
};



#endif

