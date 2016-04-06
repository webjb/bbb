// bob sang robot

#include "canbus.h"

#define DEBUG_ENABLE 0

s_can_t::s_can_t()
{
	m_socket = -1;
	init();
}

s_can_t::~s_can_t()
{
	if( m_socket >= 0 )
	{
		PRINT_INFO("close socket\n");
		close(m_socket);
	}
}

int s_can_t::init()
{
	int ret;
	struct sockaddr_can addr;
	struct ifreq ifr;
	PRINT_INFO("s_can_t::init\n");
	
	/* open socket */
	m_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if( m_socket < 0) 
	{
		PRINT_INFO("open CAN socket ERROR %d\n", m_socket);
		return m_socket;
	}

	addr.can_family = PF_CAN;
//	strncpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name));

	strcpy(ifr.ifr_name, "can0");
	ret = ioctl(m_socket, SIOCGIFINDEX, &ifr);
	if( ret < 0) 
	{
		PRINT_INFO("SIOCGIFINDEX ERROR %d\n", ret);
		return ret;
	}
	addr.can_ifindex = ifr.ifr_ifindex;


	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
//	setsockopt(m_socket, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	ret = bind(m_socket, (struct sockaddr *)&addr, sizeof(addr));
	if( ret < 0) 
	{
		PRINT_ERROR("bind ERROR %d\n", ret);
		return ret;
	}
	return 0;
}

int s_can_t::start()
{
	s_object_t::start();
	return 0;
}

int s_can_t::stop()
{
	s_object_t::stop();
	
	return 0;
}

int s_can_t::run()
{
	while(!m_quit)
	{
//		J1939_MESSAGE msg;
//        receive(&msg);
		usleep(1000*100);
	}
	m_quit = 2;
	return 0;
}

int s_can_t::send(J1939_MESSAGE * msg)
{
	int len;
	unsigned int can_id;
	char buf[100];
	char buf2[100];
	char sz[200];
	char * pbuf;
	int i;

	struct can_frame frame = {
		.can_id = 1,
	};
	
	if( m_socket < 0 )
	{
		init();
		if( m_socket < 0 )
		{
			PRINT_ERROR("send ERROR can't init socket\n");
			return -1;
		}
	}

	frame.can_dlc = msg->Msg.DataLength;
	len = sizeof(canid_t);
	memcpy((void*)buf2, (void*) msg->Array, len);
	for(int j=0; j<len; j++)
	{
		buf[j] = buf2[len-1-j];
	}
	memcpy(&can_id, buf, len);
	
	frame.can_id = can_id;
	frame.can_id &= CAN_EFF_MASK;
	frame.can_id |= CAN_EFF_FLAG;

//	frame.can_id &= CAN_EFF_MASK;
	memcpy((void*)frame.data,(void*) msg->Msg.Data, 8);
	pbuf = sz;
	for( i=0;i<8;i++)	
	{
		sprintf(pbuf, "[%02x] ", (unsigned char)msg->Msg.Data[i]);
		pbuf += strlen(pbuf);
	}
	
//	PRINT_INFO("send j1939 id:0x%08x %s\n", frame.can_id, sz);
	
	len = write(m_socket, &frame, sizeof(frame));
	if (len == -1) 
	{
		PRINT_INFO("send ERROR (%d)\n", errno);
		switch (errno) 
		{
			case ENOBUFS: 
			{
#if 0
				struct pollfd fds = {
					.fd = s,
					.events = POLLOUT,
				};
		
				if (!use_poll) {
					perror("write");
					exit(EXIT_FAILURE);
				}
		
				ret = poll(&fds, 1, 1000);
				if (ret == -1 && errno != -EINTR) {
					perror("poll()");
					exit(EXIT_FAILURE);
				}
#endif			
			}
			case EINTR: /* fallthrough */
				break;
			default:
				perror("write");
				exit(EXIT_FAILURE);
		}
	}
	else
	{
#if DEBUG_ENABLE	
		PRINT_INFO("send OK\n");
#endif
	}

#if 0	
	required_mtu = CAN_MTU;
	frame.can_id = 0x100;
	frame.len = 8;
	memset(frame.data, 0, 8);
	
	for(i=0; (i<8) && (i<len); i++ )
	{
		frame.data[i] = buf[i];
	}
	ret = write(m_socket, &frame, required_mtu);
	if( ret != required_mtu) 
	{
		PRINT_ERROR("send ERROR required_mtu != %d\n", ret);
		return -1;
	}
#endif	
	return 0;
	
}

#define BUF_SIZ	(255)

int s_can_t::receive(J1939_MESSAGE * msg)
{
	int n = 0;
	int nbytes, i;
	char buf[BUF_SIZ];
	
	struct can_frame frame = {
		.can_id = 1,
	};

//	PRINT_INFO("s_can_t::receive\n");
	
	if( m_socket < 0 )
	{
		init();
		if( m_socket < 0 )
		{
			PRINT_ERROR("send ERROR can't init socket\n");
			return -1;
		}
	}

	if ((nbytes = read(m_socket, &frame, sizeof(struct can_frame))) < 0) 
	{
		printf("read error %d\n", nbytes);
		return -1;
	} 
	else 
	{
		int len;
//		printf("read count %d\n", nbytes);
		if (frame.can_id & CAN_EFF_FLAG)
			n = snprintf(buf, BUF_SIZ, "<0x%08x> ", frame.can_id & CAN_EFF_MASK);
		else
			n = snprintf(buf, BUF_SIZ, "<0x%03x> ", frame.can_id & CAN_SFF_MASK);
	
		n += snprintf(buf + n, BUF_SIZ - n, "[%d] ", frame.can_dlc);
		for (i = 0; i < frame.can_dlc; i++) {
			n += snprintf(buf + n, BUF_SIZ - n, "%02x ", frame.data[i]);
		}
		if (frame.can_id & CAN_RTR_FLAG)
			n += snprintf(buf + n, BUF_SIZ - n, "remote request");
#if DEBUG_ENABLE
		printf("recv: cnt:%d %s ---n", nbytes, buf);
#endif
		n = 0;
		
		len = sizeof(canid_t);
		memcpy(buf, (void*)&frame.can_id, len);
		for(int j=0;j<len;j++)
		{
			msg->Array[j] = buf[len-j-1];
#if DEBUG_ENABLE			
			printf("%x ", msg->Array[j]);
#endif
		}
#if DEBUG_ENABLE		
		printf("\n");
#endif
		memcpy(msg->Msg.Data, frame.data, frame.can_dlc);
	}

	return 0;
}


