/*
* ClientProxyNode.cpp
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#include "ClientProxyNode.h"
#include "CimabueServer.h"


ClientProxyNode::ClientProxyNode(CimabueServer *caller, unsigned short port, bool enablePing)
		: ActiveNode(port)
{
	parent = caller;

	pingClient_is_running = false;
	ping_enabled = enablePing;

	if (ping_enabled)
	{
		// Ping connected clients
		if (pthread_create(&pingClient_tid, NULL, do_pingClient, this) != 0)
		{
			log.print(LOG_ERROR, "[!] Error creating pinging thread\n");
		}
	}
}

ClientProxyNode::~ClientProxyNode()
{
	if (pingClient_is_running)
	{
		// Join pingProxy thread
		if (pthread_join(pingClient_tid, NULL) != 0)
			log.print(LOG_ERROR, "[!] Error joining proxy_ping thread\n");
	}
}

// TODO: Modify in order to ping just our client
// --> If the ping fails, notify our Parent.
void* ClientProxyNode::do_pingClient(void *myself)
{
	ClientProxyNode *me = (ClientProxyNode*) myself;
	me->pingClient_is_running = true;

	/*
	while (!me->die)
{
		// Send a PING_PROXY message to each connected client
		map<string, string>::iterator client_iter;
		pthread_mutex_lock(&me->clientNameList_mutex);

		for (client_iter = me->clientNameToIPMap.begin(); client_iter
		        != me->clientNameToIPMap.end(); client_iter++)
		{
			me->log.print(LOG_PARANOID, "[ ] Pinging %s...\n",
			              client_iter->first.c_str());

			PingMessage ping(MSG_VOID, client_iter->first,
			                 me->name, MSG_VOID,
			                 MSG_VOID, MSG_VOID);

			// Get Client IP and port
			string dest_ip = client_iter->second;
			unsigned int dest_port;
			me->parseAddress(&dest_ip, &dest_port);

			Message *pong = ping.Send(dest_ip, dest_port);

			if (!pong->isErrorMessage())
			{
				// Update timestamp
				map<string, timestamp_t>::iterator ping_iter;
				pthread_mutex_lock(&me->clientPingList_mutex);
				ping_iter = me->clientPingList.find(client_iter->first);

				if (ping_iter == me->clientPingList.end())
					me->log.print(LOG_ERROR,
					              "[!] Client not found in ping list!\n");

				ping_iter->second = me->getTimestamp();

				me->log.print(LOG_PARANOID,
				              "[ ] Updated timestamp: %s - %ld\n",
				              ping_iter->first.c_str(), ping_iter->second);
				pthread_mutex_unlock(&me->clientPingList_mutex);
			}
			else
				me->log.print(LOG_ERROR, "[!] Error pinging %s: %s\n",
				              client_iter->first.c_str(),
				              ((ErrorMessage*) pong)->getErrorMessage().c_str());

			delete pong;
		}
		pthread_mutex_unlock(&me->clientNameList_mutex);

		// Delete timed-out clients
		map<string, timestamp_t>::iterator ping_iter;
		pthread_mutex_lock(&me->clientPingList_mutex);

		for (ping_iter = me->clientPingList.begin(); ping_iter
		        != me->clientPingList.end(); ping_iter++)
		{
			timestamp_t now = me->getTimestamp();
			timestamp_t delta = now - ping_iter->second;

			if (delta > 5 * 1E9)
			{
				me->log.print(LOG_INFO, "[@] Client %s timed-out (%lld)\n",
				              ping_iter->first.c_str(), delta);

				// Remove Client from related lists
				pthread_mutex_unlock(&me->clientPingList_mutex);
				me->updateClientListRem(ping_iter->first);
				me->removeClient(ping_iter->first);
				pthread_mutex_lock(&me->clientPingList_mutex);

			}
		}
		pthread_mutex_unlock(&me->clientPingList_mutex);

		sleep(5);
}

	*/

	me->pingClient_is_running = false;
	pthread_exit(0);
}
