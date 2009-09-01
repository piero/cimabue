/*
* NodeProxy.cpp
*
*  Created on: Aug 27, 2009
*      Author: piero
*/

#include "NodeProxy.h"

NodeProxy::NodeProxy(unsigned short port, long select_timeout)
        : Node(port, select_timeout), Subscriber()
{}

NodeProxy::~NodeProxy()
{}

/*
void NodeProxy::publish(Message *event)
{}

void NodeProxy::subscribe(std::string event_type, Filter *filter, Subscriber *subscriber)
{}

void NodeProxy::unsubscribe(std::string event_type, Filter *filter, Subscriber *subscriber)
{}
*/

void NodeProxy::inform(Message *event)
{}
