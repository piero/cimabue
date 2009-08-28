/*
* ErrorMessage.cpp
*
*  Created on: Jul 19, 2009
*      Author: piero
*/

#include "ErrorMessage.h"

using namespace std;


ErrorMessage::ErrorMessage(string error_msg)
:Message(MSG_ERROR)
{
	data = error_msg;

	log.print(LOG_DEBUG, "[o] ErrorMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}

ErrorMessage::ErrorMessage(string fromClient,
                           string toClient,
                           string fromServer,
                           string toServer,
                           string content,
                           string error_msg,
                           message_priority_t msg_priority)
        : Message(MSG_ERROR, fromClient, toClient, fromServer, toServer, content, msg_priority)
{
	data = error_msg;

	log.print(LOG_DEBUG, "[o] ErrorMessage (0x%x)\n\tdata: \"%s\"\n", this, data.c_str());
}


ErrorMessage::~ErrorMessage()
{
	log.print(LOG_DEBUG, "[x] ErrorMessage (0x%x)\n", this);
}


void ErrorMessage::setErrorMessage(string error_msg)
{
    data = error_msg;
}


string ErrorMessage::getErrorMessage()
{
	return data;
}

void ErrorMessage::parseData()
{
	dataParsed = true;
}
