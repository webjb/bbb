//bob sang robot
#ifndef __DRIVEPLAYER_H__
#define __DRIVEPLAYER_H__

#include "common.h"
#include "object.h"

#include "eye.h"
#include "arm.h"
#include "wheel.h"


class s_drive_player_t : public s_object_t
{
public:
	s_drive_player_t();
	~s_drive_player_t();
public:	
    static s_drive_player_t* get_inst();
    virtual int start();
    virtual int stop();
    virtual int run();	

	int on_location(char * msg);

private:
	int parse(char *msg);
    int m_run_state;
	int m_exit;
    s_wheel_t * m_wheel;

};

#endif

