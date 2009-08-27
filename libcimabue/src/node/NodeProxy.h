/*
* NodeProxy.h
*
*  Created on: Aug 27, 2009
*      Author: piero
*/

#ifndef NODEPROXY_H_
#define NODEPROXY_H_

#include "Node.h"
#include "Subscriber.h"

class NodeProxy : public Node, public Subscriber
{
	NodeProxy(unsigned short port, long select_timeout = 5);
	~NodeProxy();

    // Event Notifier pattern functions
    void publish(Event *event);
    void subscribe(std::string event_type, Filter *filter, Subscriber *subscriber);
    void unsubscribe(std::string event_type, Filter *filter, Subscriber *subscriber);
    void inform(Event *event);
};


#endif /* NODEPROXY_H_ */
