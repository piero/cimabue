/*
* ClientNode.cpp
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#include "ClientNode.h"

ClientNode::ClientNode()
		: Node()
{
	server_proxy = NULL;

}

ClientNode::~ClientNode()
{
	if (server_proxy != NULL)
		delete server_proxy;
}
