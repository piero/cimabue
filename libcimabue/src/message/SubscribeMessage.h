/*
* SubscribeMessage.h
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#ifndef SUBSCRIBEMESSAGE_H_
#define SUBSCRIBEMESSAGE_H_

#include "Message.h"

class SubscribeMessage : public Message
{
public:
	SubscribeMessage(std::string event_type, std::string node_name);
	virtual ~SubscribeMessage();

	std::string getEventType();
	std::string getNodeName();

protected:
	std::string eventType;
	std::string nodeName;

	virtual void parseData();
};


#endif /* SUBSCRIBEMESSAGE_H_ */
