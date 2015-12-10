#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include "servo.h"
//#include "brain.h"
#include "app.h"


void signal_process(int signo, siginfo_t *siginfo, void *context)
{
	PRINT_INFO("signo:%d\n", signo);
	switch(signo)
	{
	case SIGHUP:
		PRINT_INFO("SIGHUP signal\n");
		break;
	case SIGINT:
	{
		s_app_t * s_app = s_app_t::get_app();
		PRINT_INFO("SIGINT signal\n");
		if( s_app)
		{
			s_app->stop();
		}
//		exit(signo);
	}
		break;
	case SIGQUIT:
		PRINT_INFO("SIGQUIT signal\n");
		break;
	case SIGILL:
		PRINT_INFO("SIGILL signal\n");
		break;
	case SIGTRAP:
		PRINT_INFO("SIGTRAP signal\n");
		break;
	case SIGIOT: //SIGABRT
		PRINT_INFO("SIGIOT signal.\n");
		exit(signo);
		break;
	case SIGFPE:
		PRINT_INFO("SIGFPE signal.\n");
		exit(signo);
		break;
	case SIGBUS:
		PRINT_INFO("SIGBUS signal.\n");
		exit(signo);
		break;
	case SIGSEGV:
		PRINT_INFO("SIGSEGV signal.\n");
		exit(signo);
		break;
	case SIGSYS:
		PRINT_INFO("SIGSYS signal.\n");
		break;
	case SIGPIPE:
		PRINT_INFO("SIGPIPE signal.\n");
		break;
	default:
		printf("XCT get unknown signal: %d.\n", signo);
		exit(signo);
		break;
	}
    
}

void ttt()
{
	int angle;
	int distance;

	get_angle(100, -75, &angle, &distance);

	PRINT_INFO("angle=%d dis=%d\n", angle, distance);
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
		usleep(1000*100);
	}

	delete s_app;
	printf("EXIT\n");
	
	
	return 0;
}
