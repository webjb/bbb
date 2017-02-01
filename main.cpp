#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "app.h"

#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

void signal_process(int signo, siginfo_t *siginfo, void *context)
{
	s_log_info("signo:%d\n", signo);
	switch(signo)
	{
	case SIGHUP:
		s_log_info("SIGHUP signal\n");
		break;
	case SIGINT:
	{
		s_app_t * s_app = s_app_t::get_app();
		s_log_info("SIGINT signal\n");
		if( s_app)
		{
			s_app->stop();
		}
//		exit(signo);
	}
		break;
	case SIGQUIT:
		s_log_info("SIGQUIT signal\n");
		break;
	case SIGILL:
		s_log_info("SIGILL signal\n");
		break;
	case SIGTRAP:
		s_log_info("SIGTRAP signal\n");
		break;
	case SIGIOT: //SIGABRT
		s_log_info("SIGIOT signal.\n");
		exit(signo);
		break;
	case SIGFPE:
		s_log_info("SIGFPE signal.\n");
		exit(signo);
		break;
	case SIGBUS:
		s_log_info("SIGBUS signal.\n");
		exit(signo);
		break;
	case SIGSEGV:
		s_log_info("SIGSEGV signal.\n");
		exit(signo);
		break;
	case SIGSYS:
		s_log_info("SIGSYS signal.\n");
		break;
	case SIGPIPE:
		s_log_info("SIGPIPE signal.\n");
		break;
	default:
		s_log_info("XCT get unknown signal: %d.\n", signo);
		exit(signo);
		break;
	}
    
}

void ttt()
{
	int angle;
	int distance;

	get_angle(100, -75, &angle, &distance);

	s_log_info("angle=%d dis=%d\n", angle, distance);
}

int main()
{
	s_app_t * s_app;

	struct sigaction temp;
	struct sigaction action;

	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = signal_process;

	sigaction(SIGINT, &action, &temp);
	sigaction(SIGIOT, &action, &temp);
	sigaction(SIGBUS, &action, &temp);
	sigaction(SIGSEGV, &action, &temp);
	sigaction(SIGFPE, &action, &temp);
	
	s_app = s_app_t::get_app();

//	s_app->start();
	while( !s_app->is_quit())
	{
		s_sleep_ms(100);
	}

	delete s_app;
	s_log_info("EXIT\n");
	
	
	return 0;
}
