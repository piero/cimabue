/*
* PingMessage.h
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#ifndef PINGMESSAGE_H_
#define PINGMESSAGE_H_

#include "Message.h"

class PingMessage : public Message
{
public:
	PingMessage(std::string ping_data);

	PingMessage(std::string fromClient,
	            std::string toClient,
	            std::string fromServer,
	            std::string toServer,
	            std::string content,
	            std::string ping_data,
	            message_priority_t msg_priority = MSG_PRIORITY_NORMAL);

	virtual ~PingMessage();

	std::string getData();

protected:
	void parseData();
};


#endif /* PINGMESSAGE_H_ */
