/*
* ClientNode.h
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#ifndef CLIENTNODE_H_
#define CLIENTNODE_H_

#include "Node.h"
#include "ServerProxyNode.h"

class ClientNode : public Node
{
public:
	ClientNode();
	virtual ~ClientNode();

protected:
	ServerProxyNode *server_proxy;
};


#endif /* CLIENTNODE_H_ */
