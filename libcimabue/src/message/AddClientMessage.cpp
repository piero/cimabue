/*
* AddClientMessage.cpp
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#include "AddClientMessage.h"

using namespace std;

/*
AddClientMessage::AddClientMessage(string client_nickname, string client_ip, unsigned short client_port)
		: Message(MSG_ADD_CLIENT)
{
	data_separator = "#";

	char client_port_s[8];
	sprintf(client_port_s, "%d", client_port);

	data = client_nickname;
	data += data_separator;
	data += client_ip;
	data += data_separator;
	data += client_port_s;

	log.print(LOG_DEBUG, "[o] AddClientMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}
*/

AddClientMessage::AddClientMessage(string fromClient,
                                   string client_nickname,
                                   string client_ip,
                                   unsigned short client_port)
		: Message(MSG_ADD_CLIENT, fromClient)
{
	data_separator = "#";

	char client_port_s[8];
	sprintf(client_port_s, "%d", client_port);

	data = client_nickname;
	data += data_separator;
	data += client_ip;
	data += data_separator;
	data += client_port_s;

	log.print(LOG_DEBUG, "[o] AddClientMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

AddClientMessage::~AddClientMessage()
{
	log.print(LOG_DEBUG, "[x] AddClientMessage (0x%x)\n", this);
}

string AddClientMessage::getClientNickname()
{
	if (!dataParsed)
		parseData();

	return clientNickname;
}

string AddClientMessage::getClientIP()
{
	if (!dataParsed)
		parseData();

	return clientIP;
}

unsigned short AddClientMessage::getClientPort()
{
	if (!dataParsed)
		parseData();

	return clientPort;
}

void AddClientMessage::parseData()
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

	clientNickname = tokens.front();
	tokens.pop_front();
	clientIP = tokens.front();
	tokens.pop_front();
	clientPort = (unsigned short)atoi(tokens.front().c_str());
	tokens.pop_front();

	dataParsed = true;
}
