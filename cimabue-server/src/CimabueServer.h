/*
* Server.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef CIMABUESERVER_H_
#define CIMABUESERVER_H_

#include <node/ActiveNode.h>
#include "ClientProxyNode.h"

typedef enum
{
    SERVER_ROLE_MASTER,
    SERVER_ROLE_BACKUP,
    SERVER_ROLE_IDLE
} server_role_t;

typedef struct
{
	std::string ip;
	unsigned short port;
}
server_t;

typedef struct
{
	std::string nickname;
	std::string ip;
	unsigned short port;
}
client_t;


class CimabueServer : public ActiveNode
{
public:
	CimabueServer(unsigned short port = NODE_PORT_SERVER_DOWN);
	virtual ~CimabueServer();

	unsigned int getServerPort();

	server_role_t getRole();

private:
	static void* do_listen(void *myself);
	int processMessage(Message *msg, int skt);

	// Execute requests
	Message* executeSendMessage(SendMessage *msg);
	Message* executeAddClient(AddClientMessage *msg);

	void updateClientListAdd(Message *msg);
	void updateClientListRem(std::string node_name);

	// Association ClientName --> ClientProxyNode
	std::map<std::string, ClientProxyNode*> client_proxies;

	unsigned short next_port;

	// Do I have the Client in my list?
	bool haveClient(std::string clientName);

	bool getClient(std::string clientName, client_t *client);
	void removeClient(std::string clientName);

	bool getServer(std::string serverName, server_t *server);

	// Association: ServerName -> Server structure (ip, port)
	std::map<std::string, server_t> serverList;
	pthread_mutex_t serverList_mutex;

	// Association: ClientName -> Client structure (nickname, ip, port)
	std::map<std::string, client_t> clientList;
	pthread_mutex_t clientList_mutex;

	server_role_t role;
};


#endif /* CIMABUESERVER_H_ */
