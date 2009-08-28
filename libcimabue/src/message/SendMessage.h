/*
* SendMessage.h
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#ifndef SENDMESSAGE_H_
#define SENDMESSAGE_H_

#include "Message.h"


class SendMessage : public Message
{
public:
	SendMessage(std::string message);

	SendMessage(std::string fromClient,
	            std::string toClient,
	            std::string fromProxy,
	            std::string toServer,
	            std::string content,
	            std::string message,
	            message_priority_t msg_priority = MSG_PRIORITY_NORMAL);

	virtual ~SendMessage();

	void setMessage(std::string message);
	std::string getMessage();

protected:
	virtual void parseData();
};


#endif /* SENDMESSAGE_H_ */
