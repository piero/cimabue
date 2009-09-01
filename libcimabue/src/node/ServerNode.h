/*
* ServerNode.h
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#ifndef SERVERNODE_H_
#define SERVERNODE_H_

#include "ActiveNode.h"
#include "ClientProxyNode.h"


class ServerNode : public ActiveNode
{
public:
	ServerNode(unsigned short port, long select_timeout = 5);
	virtual ~ServerNode();

protected:
	std::map<std::string, ClientProxyNode*> client_proxies;
};


#endif /* SERVERNODE_H_ */
