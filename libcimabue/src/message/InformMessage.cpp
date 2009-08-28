/*
* InformMessage.cpp
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#include "InformMessage.h"

using namespace std;


InformMessage::InformMessage(string event_type, string event_data)
		: Message(MSG_INFORM)
{
	data_separator = "#";

	data = event_type;
	data += data_separator;
	data += event_data;

	log.print(LOG_DEBUG, "[o] InformMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

InformMessage::~InformMessage()
{
	log.print(LOG_DEBUG, "[x] InformMessage (0x%x)\n", this);
}

string InformMessage::getEventType()
{
	if (!dataParsed)
		parseData();

	return eventType;
}

string InformMessage::getEventData()
{
	if (!dataParsed)
		parseData();

	return eventData;
}

void InformMessage::parseData()
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
	eventData = tokens.front();
	tokens.pop_front();

	dataParsed = true;
}
