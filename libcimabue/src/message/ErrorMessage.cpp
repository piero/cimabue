/*
* ErrorMessage.cpp
*
*  Created on: Jul 19, 2009
*      Author: piero
*/

#include "ErrorMessage.h"

using namespace std;


ErrorMessage::ErrorMessage()
:Message()
{
	type = MSG_ERROR;
}

ErrorMessage::ErrorMessage(string error_msg)
:Message()
{
	type = MSG_ERROR;
	data = error_msg;
}

ErrorMessage::ErrorMessage(string fromClient,
                           string toClient,
                           string fromServer,
                           string toServer,
                           string content,
                           message_priority_t msg_priority)
        : Message(MSG_ERROR, fromClient, toClient, fromServer, toServer, content, msg_priority)
{}


ErrorMessage::~ErrorMessage()
{}


void ErrorMessage::setErrorMessage(string error_msg)
{
    data = error_msg;
}


string ErrorMessage::getErrorMessage()
{
	return data;
}
