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


#define NEXT_TURN_LEFT	1
#define NEXT_TURN_RIGHT 2

#define MAX_LINE_GROUP 	30


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


typedef struct s_line_
{
	s_point_t m_start;
	s_point_t m_end;
	int m_alpha;
	int m_dis;
	int m_lr;	
} s_line;

typedef struct s_line_group_
{
	int m_count;
	s_line m_lines[40];
	int m_avg_alpha;
	int m_avg_dis;
	int m_lr;

	s_point_t m_point_bot;
	s_point_t m_point_top;

	int m_type;
	
} s_line_group;


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
	
	int detect_direction(s_line_group * pline_group[]);
	int drive();
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
	
	int64 m_last_time;

	s_line_group m_lane_left;
	s_line_group m_lane_right;
	s_line_group m_lane_left_top;
	s_line_group m_lane_right_top;

};

#endif

