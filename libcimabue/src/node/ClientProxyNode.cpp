/*
* ClientProxyNode.cpp
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#include "ClientProxyNode.h"
#include "ServerNode.h"


ClientProxyNode::ClientProxyNode(ServerNode *caller, unsigned short port, long select_timeout)
		: ActiveNode(port, select_timeout)
{
	parent = caller;

}

ClientProxyNode::~ClientProxyNode()
{
	// TODO Auto-generated destructor stub
}
