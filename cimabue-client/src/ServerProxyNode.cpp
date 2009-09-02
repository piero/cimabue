/*
* ServerProxyNode.cpp
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#include "ServerProxyNode.h"
#include "ClientNode.h"

ServerProxyNode::ServerProxyNode(ClientNode *caller, unsigned short port, long select_timeout)
		: ActiveNode(port, select_timeout)
{
	parent = caller;

}

ServerProxyNode::~ServerProxyNode()
{
	// TODO Auto-generated destructor stub
}
