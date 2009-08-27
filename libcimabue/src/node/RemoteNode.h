/*
 * RemoteNode.h
 *
 *  Created on: Aug 27, 2009
 *      Author: piero
 */

#ifndef REMOTENODE_H_
#define REMOTENODE_H_

#include "Node.h"

class RemoteNode : public Node
{
public:
	RemoteNode(unsigned short port, long select_timeout = 5);
	~RemoteNode();

	int processMessage(Message *msg, int skt);
};


#endif /* REMOTENODE_H_ */
