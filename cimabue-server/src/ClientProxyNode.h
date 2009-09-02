/*
* ClientProxyNode.h
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#ifndef CLIENTPROXYNODE_H_
#define CLIENTPROXYNODE_H_

#include <node/ActiveNode.h>

class CimabueServer;

class ClientProxyNode : public ActiveNode
{
public:
	ClientProxyNode(CimabueServer *caller, unsigned short port, bool enablePing = false);
	virtual ~ClientProxyNode();

protected:
	CimabueServer *parent;

	// Ping connected client
	pthread_t pingClient_tid;
	bool pingClient_is_running;
	static void* do_pingClient(void *myself);
	bool ping_enabled;
};


#endif /* CLIENTPROXYNODE_H_ */
