//bob sang
#include "tcp.h"
#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

#define LISTENQ        (1024)   /*  Backlog for listen()   */

s_tcp_t::s_tcp_t(int port)
{
	s_log_info("s_tcp_t::s_tcp_t() port:%d server\n", port);
	m_is_server = 1; // it is client
	m_port = port;

	m_callback = NULL;
	m_callback_parent = NULL;
}

s_tcp_t::s_tcp_t(int port, char * ipaddr)
{
	s_log_info("s_tcp_t::s_tcp_t port:%d addr:%s clint\n", port, ipaddr);
	m_is_server = 0;
	m_port = port;
	strcpy(m_ipaddr, ipaddr);		
	m_callback = NULL;
	m_callback_parent = NULL;
}

s_tcp_t::~s_tcp_t()
{
}

int s_tcp_t::set_callback(s_callback_t callback, void * parent)
{
	m_callback = callback;
	m_callback_parent = parent;
	return 0;
}

int s_tcp_t::start_server()
{
	int flags;
	int ret;
	struct    sockaddr_in servaddr;
	s_log_info("start_server \n");
	m_socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if( m_socket_id < 0 )
	{
		s_log_info("open new socket ERROR\n");
	}
	// set it to nonblocking
	flags = fcntl(m_socket_id, F_GETFL, 0);
 	if (flags < 0) 
 	{
 		s_log_info("ERROR start_server() can't get non-block %d\n", flags);
		return -1;
 	}
  	flags |= O_NONBLOCK;
   	ret = fcntl(m_socket_id, F_SETFL, flags);
	if( ret != 0 )
   	{
   		s_log_info("ERROR start_server() can't set non-block %d errno(%d)\n", ret, errno);
		return ret;
   	}


	s_memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(m_port);

    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(m_socket_id, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) 
	{
		s_log_info("ECHOSERV: Error calling bind()\n");
		return -1;
    }

    if ( listen(m_socket_id, LISTENQ) < 0 ) 
	{
		s_log_info("ECHOSERV: Error calling listen()\n");
		return -1;
    }
	return 0;	
}

int s_tcp_t::start()
{
	if( m_is_server )
		start_server();
	s_object_t::start();
	return 0;
}

int s_tcp_t::stop()
{
	s_object_t::stop();
	return 0;
}

void * s_tcp_t::thread_entry(void * para)
{
	s_tcp_t * inst;	
	int socket_id;
	s_tcp_connection_t * connection;
	connection = (s_tcp_connection_t*) para;
	inst = (s_tcp_t*)connection->m_parent;
	socket_id = connection->m_socket_id;
	inst->run_server(socket_id);
	return 0;
}

int s_tcp_t::run_server(int socket_id)
{
	char * buffer;
	int len;
	buffer = new char[1024];
	s_log_info("run_server ENTER\n");
	while( !m_quit)
	{
		len = read(socket_id, buffer, 1024);
		if( len <= 0 )
		{
			if( errno == EINTR)
			{
				s_sleep_ms(1);
				continue;
			}
			else
			{
				PRINT_INFO("DONE\n");
				break;
			}
		}
		else
		{
			if( m_callback)
			{
				s_buffer_t buf;
				buf.m_buffer = buffer;
				buffer[len] = 0;
				buf.m_length = len;
				s_log_info("recv: %s\n", buffer);
				m_callback(m_callback_parent, &buf);
			}
			
//			buffer[len] = 0;
		}
	}
	
	if ( close(socket_id) < 0 ) 
	{
		s_log_info("ERROR calling close()\n");
	}
	delete buffer;
//	PRINT_INFO("run_server() EXIT\n");
	return 0;
}

int s_tcp_t::run()
{
	int       conn_s;
	s_log_info("s_tcp_t::run() ENTER\n");
	while ( !m_quit)
	{
		if( m_is_server )
		{		
			conn_s = accept(m_socket_id, NULL, NULL);
			if( conn_s < 0 )
			{
				if( (errno == EWOULDBLOCK) || (errno == EAGAIN) )// || (conn_s == -1) )
				{
					usleep(1000);
					continue;
				}
				else
				{
					s_log_info("accept() closed %d\n", errno);					
					break;
				}
			}
			s_log_info("find new client connected\n");
			s_tcp_connection_t * connection;
			connection = new s_tcp_connection_t;
			connection->m_parent = this;
			connection->m_socket_id = conn_s;
			CREATE_THREAD(m_connection_thread, thread_entry, 0x8000, connection, 1);	
		}
	}
	m_quit = 2;
	s_log_info("s_tcp_t::run() EXIT\n");
	return 0;
}

