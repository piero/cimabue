/*
* UnsubscribeMessage.cpp
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#include "UnsubscribeMessage.h"

using namespace std;


UnsubscribeMessage::UnsubscribeMessage(string event_type, string node_name)
		: Message(MSG_UNSUBSCRIBE)
{
	data_separator = "#";

	data = event_type;
	data += data_separator;
	data += node_name;

	log.print(LOG_DEBUG, "[o] UnsubscribeMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

UnsubscribeMessage::~UnsubscribeMessage()
{
	log.print(LOG_DEBUG, "[x] UnsubscribeMessage (0x%x)\n", this);
}

string UnsubscribeMessage::getEventType()
{
	if (!dataParsed)
		parseData();

	return eventType;
}

string UnsubscribeMessage::getNodeName()
{
	if (!dataParsed)
		parseData();

	return nodeName;
}

void UnsubscribeMessage::parseData()
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
