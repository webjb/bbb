//bob sang robot
#ifndef __DRIVEPLAYER_H__
#define __DRIVEPLAYER_H__

#include "common.h"
#include "object.h"

#include "eye.h"
#include "arm.h"
#include "wheel.h"

#define DRV_STATE_STOP	1
#define DRV_STATE_FWD	2
#define DRV_STATE_TURN_LEFT	3
#define DRV_STATE_TURN_RIGHT 4

typedef struct s_point_t_
{
	int m_x;
	int m_y;
} s_point_t;


typedef struct s_lane_xy_t_
{
	s_point_t m_points[2];
	int m_alpha;
	int m_dis;
	int m_lr; // left or right
} s_lane_xy_t;

typedef struct s_lane_loc_t_
{
	s_lane_xy_t m_left;
	s_lane_xy_t m_right;
	s_lane_xy_t m_top;
} s_lane_loc_t;

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
	
	int drive(s_lane_loc_t lane);
	int on_location(char * msg);

private:
	int parse(char *msg);
    int m_run_state;
	int m_exit;
    s_wheel_t * m_wheel;
    s_eye_t * m_eye;

	int m_next_turn;
	list<s_lane_loc_t> m_loc_list;

};

#endif

