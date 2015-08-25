//bob sang robot

#ifndef __S_TCP_H__
#define __S_TCP_H__

#include "common.h"
#include "object.h"

typedef struct s_tcp_connection_t_
{
    void * m_parent;
    int m_socket_id;
    
} s_tcp_connection_t;

class s_tcp_t : public s_object_t
{
public:
    s_tcp_t(int port);
    s_tcp_t(int port, char * ipaddr);
    ~s_tcp_t();

    int start_server();

    int set_callback(s_callback_t callback, void * parent);
public:
    virtual int start();
    virtual int stop();
    virtual int run();

	int send(char * pbuf, int len);
    
    static void * thread_entry(void *);

    int run_server(int sock_id);
    
public:
	int connect();
	int receive();
private:
    int m_is_server;
    int m_port;
    char m_ipaddr[80];
    int m_socket_id;
    
    thread_t m_connection_thread;

    s_callback_t m_callback;
    void * m_callback_parent;
    
};

#endif

