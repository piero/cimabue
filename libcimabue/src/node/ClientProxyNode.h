/*
* ClientProxyNode.h
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#ifndef CLIENTPROXYNODE_H_
#define CLIENTPROXYNODE_H_

#include "ActiveNode.h"

class ServerNode;

class ClientProxyNode : public ActiveNode
{
public:
	ClientProxyNode(ServerNode *caller, unsigned short port, long select_timeout = 5);
	virtual ~ClientProxyNode();

protected:
	ServerNode *parent;
};


#endif /* CLIENTPROXYNODE_H_ */
