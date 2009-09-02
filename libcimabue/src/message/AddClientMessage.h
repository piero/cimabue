/*
* AddClientMessage.h
*
*  Created on: 28 Aug 2009
*      Author: piero
*/

#ifndef ADDCLIENTMESSAGE_H_
#define ADDCLIENTMESSAGE_H_

#include "Message.h"

class AddClientMessage : public Message
{
public:
	AddClientMessage(std::string client_nickname, std::string client_ip, unsigned short client_port);
	virtual ~AddClientMessage();

	std::string getClientNickname();
	std::string getClientIP();
	unsigned short getClientPort();

protected:
	std::string clientNickname;
	std::string clientIP;
	unsigned short clientPort;

	virtual void parseData();
};


#endif /* ADDCLIENTMESSAGE_H_ */
