/*
* MessageQueue.h
*
*  Created on: 05-Aug-2009
*      Author: piero
*/

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include "Message.h"
#include <queue>


typedef struct
{
	int socket;
	Message *msg;
}
message_entry_t;


class MessageQueue
{
public:
	MessageQueue();
	virtual ~MessageQueue();

	int pop(message_entry_t *output);
	void push(message_entry_t entry);
	unsigned int size();
	void clear();

protected:

	std::queue<message_entry_t> msg_queue;
	Log log;
};

#endif /* MESSAGEQUEUE_H_ */
