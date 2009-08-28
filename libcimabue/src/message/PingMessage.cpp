/*
* PingMessage.cpp
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#include "PingMessage.h"

using namespace std;


PingMessage::PingMessage(string ping_data)
		: Message(MSG_PING_CLIENT)
{
	data = ping_data;

	log.print(LOG_DEBUG, "[o] PingMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

PingMessage::PingMessage(string fromClient,
                         string toClient,
                         string fromServer,
                         string toServer,
                         string content,
                         string ping_data,
                         message_priority_t msg_priority)
		: Message(MSG_PING_CLIENT, fromClient, toClient, fromServer, toServer, content, msg_priority)
{
	data = ping_data;

	log.print(LOG_DEBUG, "[o] PingMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

PingMessage::~PingMessage()
{
	log.print(LOG_DEBUG, "[x] PingMessage (0x%x)\n", this);
}

string PingMessage::getData()
{
	return data;
}

void PingMessage::parseData()
{
	dataParsed = true;
}
