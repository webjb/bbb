// bob sang robot
#include "brain.h"

s_brain_t::s_brain_t()
{

}

s_brain_t::~s_brain_t()
{
}

int s_brain_t::look_for_ball()
{
	return 0;
}

int s_brain_t::move_to_ball()
{
	return 0;
}

int s_brain_t::kick_ball()
{
	// looking for a ball
	// then kick it

	return 0;
}

int s_brain_t::run()
{
	while(1)
	{
		if( !look_for_ball() )
			continue;
		if( !move_to_ball() )
			continue;
		kick_ball();
		
	}
	return 0;
}

