#ifndef __S_HEAD_H__
#define __S_HEAD_H__

class s_head_t
{
private:

public:
	int left(int pos, int speed);
	int right(int pos, int speed);
	int up(int pos, int speed);
	int down(int pos, int speed);
	int get_pos(int& pos1, int& pos2);
};

#endif
