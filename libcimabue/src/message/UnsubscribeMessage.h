/*
* UnsubscribeMessage.h
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#ifndef UNSUBSCRIBEMESSAGE_H_
#define UNSUBSCRIBEMESSAGE_H_

#include "Message.h"

class UnsubscribeMessage : public Message
{
public:
	UnsubscribeMessage(std::string event_type, std::string node_name);
	virtual ~UnsubscribeMessage();

	std::string getEventType();
	std::string getNodeName();

protected:
	std::string eventType;
	std::string nodeName;

	virtual void parseData();
};


#endif /* UNSUBSCRIBEMESSAGE_H_ */
