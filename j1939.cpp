// bob sang

#include "j1939.h"
#include "log.h"
#include "utilities.h"

using namespace s_log;
using namespace s_utilities;

#define J1939_ADDRESS	0x10

// Internal definitions

#define ADDRESS_CLAIM_TX	1
#define ADDRESS_CLAIM_RX	2

#define DEBUG_ENABLE 1

#define WHEEL_CMD		4

s_j1939_t::s_j1939_t()
{
	int i;
	m_can = new s_can_t();
	m_j1939_addr = 1;
	m_dest_addr = 10;

	i = 0;
	m_name[i++] = J1939_CA_NAME0;
	m_name[i++] = J1939_CA_NAME1;
	m_name[i++] = J1939_CA_NAME2;
	m_name[i++] = J1939_CA_NAME3;
	m_name[i++] = J1939_CA_NAME4;
	m_name[i++] = J1939_CA_NAME5;
	m_name[i++] = J1939_CA_NAME6;
	m_name[i++] = J1939_CA_NAME7;

	m_flags = 0;	

	m_last_left = -1;
	m_last_right = -1;
}

s_j1939_t::~s_j1939_t()
{
	delete m_can;
}

int s_j1939_t::start()
{
	s_object_t::start();
	return 0;
}

int s_j1939_t::stop()
{
	s_object_t::stop();
	m_last_left = -1;
	m_last_right = -1;
	return 0;
}

#define FLAG_COMMAND_ADDR			1
#define FLAG_FIRST_DATA_PACKET	2
#define FLAG_CANNOT_CLAIM_ADDR	4
#define FLAG_WAITING_CLAM_ADDR_CONTENTION	8
#define FLAG_GOT_FIRST_DATA_PACKET 		0x10
#define FLAG_GETTING_COMMANDED_ADDR 		0x20

int s_j1939_t::compare_name( unsigned char *OtherName )
{
	int i;

	for (i = 0; (i<J1939_DATA_LENGTH) && (OtherName[i] == m_name[i]); i++);

	if (i == J1939_DATA_LENGTH)
		return 0;
	else if ( m_name[i] < OtherName[i] )
		return -1;
	else
		return 1;
}

void s_j1939_t::copy_name( J1939_MESSAGE * jmsg)
{
	int i;
	for(i=0;i<J1939_DATA_LENGTH;i++)
	{
		jmsg->Msg.Data[i] = m_name[i];
	}
}

int s_j1939_t::set_address_filter(int addr)
{
	return 0;
}

void s_j1939_t::address_claim( unsigned char Mode )
{
	J1939_MESSAGE jmsg;

	jmsg.Msg.Priority = J1939_CONTROL_PRIORITY;
	jmsg.Msg.PDUFormat = J1939_PF_ADDRESS_CLAIMED;
	jmsg.Msg.PDUSpecific = J1939_GLOBAL_ADDRESS;
	jmsg.Msg.DataLength = J1939_DATA_LENGTH;

	if (Mode == ADDRESS_CLAIM_TX)
		goto SendAddressClaim;

	if (jmsg.Msg.SourceAddress != m_j1939_addr)
		return;

	if (compare_name( jmsg.Msg.Data ) != -1) // Our CA_Name is not less
	{
		// Send Cannot Claim Address message
		copy_name(&jmsg);
		jmsg.Msg.SourceAddress = J1939_NULL_ADDRESS;
		m_can->send(&jmsg);

		// Set up MCP filter 2 to receive messages sent to the global address
		set_address_filter( J1939_GLOBAL_ADDRESS );

		m_flags |= FLAG_CANNOT_CLAIM_ADDR;
		m_flags &= ~FLAG_WAITING_CLAM_ADDR_CONTENTION;
		return;
	}

SendAddressClaim:
	// Send Address Claim message for CommandedAddress
	copy_name(&jmsg);
	jmsg.Msg.SourceAddress = m_commanded_addr;
	m_can->send(&jmsg);

	if (((m_commanded_addr & 0x80) == 0) ||			// Addresses 0-127
		((m_commanded_addr & 0xF8) == 0xF8))		// Addresses 248-253 (254,255 illegal)
	{
		m_flags &= ~FLAG_CANNOT_CLAIM_ADDR;
		m_j1939_addr = m_commanded_addr;

		// Set up MCP filter 2 to receive messages sent to this address
		set_address_filter( m_j1939_addr );
	}
	else
	{
		// We don't have a proprietary address, so we need to wait.
		m_flags |= FLAG_WAITING_CLAM_ADDR_CONTENTION;
	}
}

void s_j1939_t::request_claim_addr(J1939_MESSAGE *jmsg)
{	
	if ( m_flags & FLAG_CANNOT_CLAIM_ADDR)
		jmsg->Msg.SourceAddress = J1939_NULL_ADDRESS;	// Send Cannot Claim Address message
	else
		jmsg->Msg.SourceAddress = m_j1939_addr;		// Send Address Claim for current address

	jmsg->Msg.Priority = J1939_CONTROL_PRIORITY;
	jmsg->Msg.PDUFormat = J1939_PF_ADDRESS_CLAIMED;	// Same as J1939_PF_CANNOT_CLAIM_ADDRESS
	jmsg->Msg.PDUSpecific = J1939_GLOBAL_ADDRESS;
	jmsg->Msg.DataLength = J1939_DATA_LENGTH;
	copy_name(jmsg);
	
	m_can->send(jmsg);
}

int s_j1939_t::parse(J1939_MESSAGE * jmsg)
{
//	int loop;
	J1939_MESSAGE_STRUCT * msg;
	msg = &jmsg->Msg;

//	PRINT_INFO("parse msg: src_addr:%d dst_addr:%d fmt:0x%x\n", msg->SourceAddress, msg->PDUSpecific, msg->PDUFormat);
#if 0	
	loop = (msg->PDUFormat & 0xE0) >> 3;			// Get SID2-0 ready.
	msg->PDUFormat = (msg->PDUFormat & 0x03)|loop | ((msg->PDUFormat_Top & 0x07) << 5);
#endif
	switch(msg->PDUFormat)
	{		
#if 0	
		case J1939_PF_CM_BAM:
			if ((msg->Data[0] == J1939_BAM_CONTROL_BYTE) &&
				(msg->Data[5] == J1939_PGN0_COMMANDED_ADDRESS) &&
				(msg->Data[6] == J1939_PGN1_COMMANDED_ADDRESS) &&
				(msg->Data[7] == J1939_PGN2_COMMANDED_ADDRESS))
			{
				m_flags |= FLAG_COMMAND_ADDR;
				m_commanded_addr_source = msg->SourceAddress;
			}
			break;
		case J1939_PF_DT:
			if ((m_flags & FLAG_COMMAND_ADDR ) && (m_commanded_addr_source == msg->SourceAddress))
			{
				if (( (m_flags & FLAG_FIRST_DATA_PACKET) == 0) && (msg->Data[0] == 1))
				{
					for (i=0; i<7; i++)
						CommandedAddressName[i] = msg->Data[i+1];
					m_flags |= FLAG_FIRST_DATA_PACKET;
				}
				else if ((flags & FLAG_FIRST_DATA_PACKET) && (msg->Data[0] == 2))
				{
					CommandedAddressName[7] = msg->Data[1];
					m_commanded_addr = msg->Data[2];
					if ((compare_name( CommandedAddressName ) == 0) &&	CA_AcceptCommandedAddress())
					{
						address_claim( ADDRESS_CLAIM_TX );
					}
					m_flags &= ~FLAG_GOT_FIRST_DATA_PACKET;
					m_flags &= ~FLAG_GETTING_COMMANDED_ADDR;
				}
			}
			break;
#endif			
		case J1939_PF_REQUEST:
			if ((msg->Data[0] == J1939_PGN0_REQ_ADDRESS_CLAIM) &&
				(msg->Data[1] == J1939_PGN1_REQ_ADDRESS_CLAIM) &&
				(msg->Data[2] == J1939_PGN2_REQ_ADDRESS_CLAIM))
			{
				request_claim_addr(jmsg);
			}
			break;
		case J1939_PF_ADDRESS_CLAIMED:
			address_claim( ADDRESS_CLAIM_RX );
			break;
	}
	return 0;
}

int s_j1939_t::run()
{
	int ret;
	J1939_MESSAGE msg;
	while( !m_quit)
	{
		ret = m_can->receive(&msg);
		switch (ret)
		{
			case S_ERROR_CAN_BUS_NOT_EXIST:
				s_sleep_ms(5000);
				break;
			case S_ERROR_CAN_BUS_NO_DATA:
				s_sleep_ms(50);
				break;
			case S_ERROR_NONE:
				parse(&msg);
				break;
			default:
				s_sleep_ms(50);
				break;
		}
	}
	m_quit = 2;
	return 0;
}

int s_j1939_t::send_message(J1939_MESSAGE * msg)
{
	int i;
	unsigned char crc;
	unsigned char * buf;
	
	msg->Msg.Res = 0;
	msg->Msg.RTR = 0;
	msg->Msg.DataPage = 0;
	msg->Msg.DataLength = 8;
	msg->Msg.Priority = J1939_CONTROL_PRIORITY;
	msg->Msg.SourceAddress = m_j1939_addr;
	msg->Msg.PDUSpecific = m_dest_addr;

//	PRINT_INFO("send_message PDUFormat 0x%x --> ", msg->Msg.PDUFormat);
#if 0
	msg->Msg.PDUFormat_Top = msg->Msg.PDUFormat >> 5; 	// Put the top three bits into SID5-3
	Temp = msg->Msg.PDUFormat & 0x03;						// Save the bottom two bits.
	msg->Msg.PDUFormat = (msg->Msg.PDUFormat & 0x1C) << 3;// Move up bits 4-2 into SID2-0.
	msg->Msg.PDUFormat |= Temp | 0x08;						// Put back EID17-16, set EXIDE.
#endif
//	PRINT_INFO(" 0x%x\n", msg->Msg.PDUFormat);
	
	crc = 0;
	buf = msg->Array;
	buf[J1939_MSG_LENGTH + 6 ] = 0x55;
	for(i=J1939_MSG_LENGTH;i<J1939_MSG_LENGTH+J1939_DATA_LENGTH-1; i++)
	{
		crc += buf[i];
	}
	buf[i] = crc;
//	printf("crc:0x%x\n", crc);
	m_can->send(msg);
	
	return 0;
}

int s_j1939_t::set_wheels(int left, int right) // 0-100%
{
	unsigned int left_speed, left_dir, right_speed, right_dir;
	J1939_MESSAGE msg;

	if( (left == m_last_left) && (right == m_last_right))
		return 0;

	PRINT_INFO("wheel: left:%d right:%d\n", left, right);
	m_last_left = left;
	m_last_right = right;
	
	memset(&msg, 0, sizeof(msg));

//	right = -1* right;
	if( left > 100 )
		left = 100;
	if( left < -100 )
		left = -100;

	if( right > 100 )
		right = 100;
	if( right < -100 )
		right = -100;

	if( left >= 0 )
	{
		left_speed = left*5;
		left_dir = 0;
	}
	else
	{
		left_speed = 510 + left*5;
//		left_speed = left*-5;
		left_dir = 1;
	}

	if( right >= 0 )
	{
		right_speed = right*5;
		right_dir = 0;
	}
	else
	{
		right_speed = 510 + right*5;
//		right_speed = right *-5;
		right_dir = 1;
	}
	
	msg.Msg.PDUFormat = WHEEL_CMD;
	msg.Msg.Data[0] = left_speed/256;
    msg.Msg.Data[1] = left_speed%256;
    msg.Msg.Data[2] = left_dir; // 0 -- ; 1--reverse
    msg.Msg.Data[3] = right_speed/256;
    msg.Msg.Data[4] = right_speed%256;
    msg.Msg.Data[5] = right_dir;
	
	
	send_message(&msg);
	return 0;
}

