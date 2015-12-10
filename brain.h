// bob sang robot

#ifndef __S_BRAIN_H__
#define __S_BRAIN_H__

#include "common.h"

#define S_CMD_KICK_BALL 	1
#define S_CMD_PLAY_MUSIC	2
#define S_CMD_WEATHER	3


class s_brain_t : public s_object_t
{
public:
    s_brain_t();
    ~s_brain_t();
public:
	int go_sleep();
	int wake_up();
	int look_for_ball();
	int move_to_ball();
	int kick_ball();

    int run();
public:
	int send_command(int cmd);
};

#endif
