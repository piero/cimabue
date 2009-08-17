/*
* MessageQueue.cpp
*
*  Created on: 05-Aug-2009
*      Author: piero
*/

#include "MessageQueue.h"

MessageQueue::MessageQueue()
{}

MessageQueue::~MessageQueue()
{
	clear();
}

int MessageQueue::pop(message_entry_t *output)
{
	if (msg_queue.empty())
		return -1;

	output->socket = ((message_entry_t) (msg_queue.front())).socket;
	output->msg = ((message_entry_t) (msg_queue.front())).msg;

	log.print(LOG_PARANOID, "[-] Popping message:\n");
	log.print(LOG_PARANOID, "Socket: %d\n", output->socket);
	output->msg->dump();

	msg_queue.pop();
	return 0;
}

void MessageQueue::push(message_entry_t entry)
{
	msg_queue.push(entry);

	log.print(LOG_PARANOID, "[+] Pushed message:\n");
	log.print(LOG_PARANOID, "Socket: %d\n", entry.socket);
	entry.msg->dump();
}

unsigned int MessageQueue::size()
{
	return msg_queue.size();
}

void MessageQueue::clear()
{
	while (!msg_queue.empty())
		msg_queue.pop();

	log.print(LOG_PARANOID, "[#] Cleared MessageQueue\n");
}

