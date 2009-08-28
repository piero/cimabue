/*
* InformMessage.h
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#ifndef INFORMMESSAGE_H_
#define INFORMMESSAGE_H_

#include "Message.h"

class InformMessage : public Message
{
public:
	InformMessage(std::string event_type, std::string event_data);
	virtual ~InformMessage();

	std::string getEventType();
	std::string getEventData();

protected:
	std::string eventType;
	std::string eventData;

	virtual void parseData();
};


#endif /* INFORMMESSAGE_H_ */
