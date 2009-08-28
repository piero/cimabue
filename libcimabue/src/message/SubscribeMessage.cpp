/*
* SubscribeMessage.cpp
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#include "SubscribeMessage.h"

using namespace std;


SubscribeMessage::SubscribeMessage(string event_type, string node_name)
		: Message(MSG_SUBSCRIBE)
{
	data_separator = "#";

	data = event_type;
	data += data_separator;
	data += node_name;

	log.print(LOG_DEBUG, "[o] SubscribeMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

SubscribeMessage::~SubscribeMessage()
{
	log.print(LOG_DEBUG, "[x] SubscribeMessage (0x%x)\n", this);
}

string SubscribeMessage::getEventType()
{
	if (!dataParsed)
		parseData();

	return eventType;
}

string SubscribeMessage::getNodeName()
{
	if (!dataParsed)
		parseData();

	return nodeName;
}

void SubscribeMessage::parseData()
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

	eventType = encodeType(tokens.front());
	tokens.pop_front();
	nodeName = tokens.front();
	tokens.pop_front();

	dataParsed = true;
}
