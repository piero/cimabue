/*
* SendMessage.cpp
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#include "SendMessage.h"

using namespace std;


SendMessage::SendMessage(string message)
		: Message(MSG_SEND_MESSAGE)
{
	data = message;

	log.print(LOG_DEBUG, "[o] SendMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

SendMessage::SendMessage(string fromClient,
                         string toClient,
                         string fromServer,
                         string toServer,
                         string content,
                         string message,
                         message_priority_t msg_priority)
		: Message(MSG_SEND_MESSAGE, fromClient, toClient, fromServer, toServer, content, msg_priority)
{
	data = message;

	log.print(LOG_DEBUG, "[o] SendMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

SendMessage::~SendMessage()
{
	log.print(LOG_DEBUG, "[x] SendMessage (0x%x)\n", this);
}

void SendMessage::setMessage(string message)
{
	data = message;
}


string SendMessage::getMessage()
{
	return data;
}

void SendMessage::parseData()
{
	dataParsed = true;
}
