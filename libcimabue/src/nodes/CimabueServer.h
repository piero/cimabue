/*
* Server.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef CIMABUESERVER_H_
#define CIMABUESERVER_H_

#include "Node.h"

class CimabueServer : public Node
{
public:
	CimabueServer(unsigned short port = NODE_PORT_SERVER_DOWN, bool enablePing = false);
	virtual ~CimabueServer();

	unsigned int getServerPort();

private:
	static void* do_listen(void *myself);
	int processUpMessage(Message *msg, int skt);
	int processDownMessage(Message *msg, int skt);

	// Execute requests
	Message* executeSendMessage(Message *msg);
	Message* executeAddProxy(Message *msg);
	Message* executeRemProxy(Message *msg);
	Message* executeAddClient(Message *msg);
	Message* executeRemClient(Message *msg);

	// Ping connected proxies
	pthread_t pingProxy_tid;
	bool pingProxy_is_running;
	static void* do_pingClient(void *myself);
	bool ping_enabled;

	// Do I have the Proxy in my list?
	bool haveProxy(std::string const proxyName);

	// Find the Proxy associated to a given Client
	std::string getProxyWithClient(std::string const clientName);

	// Get the IP of a given Server in serverList
	std::string getServerAddress(std::string const serverName);

	// Get the Server connected to a given Proxy
	std::string whoHasProxy(std::string const proxyName);

	// Get the IP of a given Proxy
	std::string getProxyAddress(std::string const proxyName);

	void removeProxy(std::string proxyName);

	// Association: ServerName -> Server IP address
	std::map<std::string, std::string> serverList;
	pthread_mutex_t serverList_mutex;

	// Association: ProxyName -> ServerName
	std::map<std::string, std::string> proxyServerList;
	pthread_mutex_t proxyServerList_mutex;

	// Association: ProxyName -> Proxy IP address
	std::map<std::string, std::string> proxyList;
	pthread_mutex_t proxyList_mutex;

	// Assiciation: ClientName --> ProxyName
	std::map<std::string, std::string> clientList;
	pthread_mutex_t clientList_mutex;

	// Association proxyName --> last_ping_timestamp
	std::map<std::string, timestamp_t> proxyPingList;
	pthread_mutex_t proxyPingList_mutex;
};


#endif /* CIMABUESERVER_H_ */
