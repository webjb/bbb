// bob sang
#ifndef __S_LEG_H__
#define __S_LEG_H__

#include "common.h"
#include "object.h"
#include "j1939.h"

class s_leg_t : public s_object_t
{
public:
    s_leg_t();
    ~s_leg_t();
    
public:
    virtual int start();
    virtual int stop();
    virtual int run();
public:
    int set_speed(int speed);
    int left(int speed = 0);
    int right(int speed = 0);
    int forward(int speed = 0);
    int backward(int speed = 0);
    int back_left(int speed = 0 );
    int back_right(int speed = 0);
private:
    int m_speed;

    s_j1939_t * m_j1939;
};

#endif
