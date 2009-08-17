/*
* ErrorMessage.h
*
*  Created on: Jul 19, 2009
*      Author: piero
*/

#ifndef ERRORMESSAGE_H_
#define ERRORMESSAGE_H_

#include "Message.h"

class ErrorMessage : public Message
{
public:
	ErrorMessage();

	ErrorMessage(std::string error_msg);

    ErrorMessage(std::string fromClient,
	        std::string toClient,
	        std::string fromProxy,
	        std::string toProxy,
	        std::string fromServer,
	        std::string toServer,
	        std::string content,
	        message_priority_t msg_priority = MSG_PRIORITY_NORMAL);

    virtual ~ErrorMessage();

    void setErrorMessage(std::string error_msg);
    std::string getErrorMessage();
};


#endif /* ERRORMESSAGE_H_ */
