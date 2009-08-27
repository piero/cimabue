/*
 * Subscriber.h
 *
 *  Created on: Aug 27, 2009
 *      Author: piero
 */

#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include "../Event.h"

class Subscriber
{
public:
	Subscriber() {}
	virtual ~Subscriber() {}

	virtual void inform(Event *event) = 0;
};

#endif /* SUBSCRIBER_H_ */
