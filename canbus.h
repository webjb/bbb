//bob sang robot

#ifndef __S_CAN_H__
#define __S_CAN_H__

#include "object.h"
#include "can.h"
#include "can/raw.h"

#include "j1939.h"
#include "j1939_16.h"
#include "j1939cfg.h"


class s_can_t : public s_object_t
{
public:
    s_can_t();
    ~s_can_t();
private:
    int init();
    
public:
    virtual int start();
    virtual int stop();
    virtual int run();
    
public:
	int send(J1939_MESSAGE *msg);
	int receive(J1939_MESSAGE * msg);
private:
    int m_socket;
};

#endif

