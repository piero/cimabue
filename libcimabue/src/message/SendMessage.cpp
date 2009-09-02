/*
* SendMessage.cpp
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#include "SendMessage.h"

using namespace std;


SendMessage::SendMessage(string sender_nick, string dest_nick, string message)
		: Message(MSG_SEND_MESSAGE)
{
	data_separator = "#";

	data = sender_nick;
	data += data_separator;
	data += dest_nick;
	data += data_separator;
	data += message;

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

string SendMessage::getSenderNickname()
{
	if (!dataParsed)
		parseData();

	return sender_nickname;
}

string SendMessage::getReceiverNickname()
{
	if (!dataParsed)
		parseData();

	return receiver_nickname;
}

string SendMessage::getMessage()
{
	if (!dataParsed)
		parseData();

	return message;
}

void SendMessage::parseData()
{
	string::size_type lastPos = data.find_first_not_of(data_separator, 0);
	string::size_type pos = data.find_first_of(data_separator, 0);

	list<string> tokens;

	while (string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(data.substr(lastPos, pos - lastPos));
		lastPos = data.find_first_not_of(data_separator, pos);
		pos = data.find_first_of(data_separator, lastPos);
	}

	sender_nickname = tokens.front();
	tokens.pop_front();
	receiver_nickname = tokens.front();
	tokens.pop_front();
	message = tokens.front();
	tokens.pop_front();

	dataParsed = true;
}
