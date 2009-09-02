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
	SendMessage(std::string sender_nick, std::string dest_nick, std::string message);

	SendMessage(std::string fromClient,
	            std::string toClient,
	            std::string fromServer,
	            std::string toServer,
	            std::string content,
	            std::string message,
	            message_priority_t msg_priority = MSG_PRIORITY_NORMAL);

	virtual ~SendMessage();

	void setMessage(std::string message);

	std::string getSenderNickname();
	std::string getReceiverNickname();
	std::string getMessage();

protected:
	virtual void parseData();

	std::string sender_nickname;
	std::string receiver_nickname;
	std::string message;
};


#endif /* SENDMESSAGE_H_ */
