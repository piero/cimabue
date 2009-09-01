/*
 * Subscriber.h
 *
 *  Created on: Aug 27, 2009
 *      Author: piero
 */

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include "../message/Message.h"

class Subscriber
{
public:
	Subscriber() {}
	virtual ~Subscriber() {}

	virtual void inform(Message *event) = 0;
};

#endif /* SUBSCRIBER_H_ */
