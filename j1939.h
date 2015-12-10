// bob sang

#ifndef __S_J1939_H__
#define __S_J1939_H__

#include "common.h"
#include "object.h"
#include "canbus.h"

#include "j1939_16.h"
#include "j1939cfg.h"

class s_can_t ;
class s_j1939_t : public s_object_t
{
public:
    s_j1939_t();
    virtual ~s_j1939_t();
    
public:
    virtual int start();
    virtual int stop();
    virtual int run();

public:
    
    int compare_name( unsigned char *OtherName );    
    void copy_name( J1939_MESSAGE * jmsg);
    int set_address_filter(int addr);
    void request_claim_addr(J1939_MESSAGE *jmsg);
    void address_claim( unsigned char Mode );
    
    int parse(J1939_MESSAGE * msg);
    
    int set_wheels(int left, int right);///int left_speed, int left_dir, int right_speed, int right_dir);
//    int set_led(int num, int start_or_stop);
    
private:
    int send_message(J1939_MESSAGE * msg);
    unsigned char m_j1939_addr;
    unsigned char m_dest_addr;
    s_can_t * m_can;
    char m_name[8];
    int m_flags;
    unsigned char m_commanded_addr;
    unsigned char m_commanded_addr_source;
    
};

#endif
