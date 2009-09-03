/*
* ServerProxyNode.h
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#ifndef SERVERPROXYNODE_H_
#define SERVERPROXYNODE_H_

#include <node/ActiveNode.h>

class ClientNode;

class ServerProxyNode : public ActiveNode
{
public:
	ServerProxyNode(ClientNode *caller, unsigned short port, long select_timeout = 5);
	virtual ~ServerProxyNode();

protected:
	ClientNode *parent;
};


#endif /* SERVERPROXYNODE_H_ */
