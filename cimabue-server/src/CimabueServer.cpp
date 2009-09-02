/*
* Server.cpp
*
*  Created on: May 4, 2009
*      Author: piero
*/

#include "CimabueServer.h"

using namespace std;

CimabueServer::CimabueServer(unsigned short port) :
		ActiveNode(port)
{
	role = SERVER_ROLE_IDLE;
	next_port = port + 1;

	log.print(LOG_DEBUG, "CimabueServer()\n");

	pthread_mutex_init(&serverList_mutex, NULL);
	pthread_mutex_init(&clientList_mutex, NULL);
}

CimabueServer::~CimabueServer()
{
	pthread_mutex_destroy(&serverList_mutex);
	pthread_mutex_destroy(&clientList_mutex);

	log.print(LOG_DEBUG, "[x] Server\n");
}

int CimabueServer::processMessage(Message *msg, int skt)
{
	int ret = NODE_RET_ERROR;
	Message *answer = NULL;

	log.print(LOG_INFO, "[ ] Processing message from a CLIENT\n");

	bool update_client_list = false;

	// A new client doesn't know our name, but only our IP address
	if (msg->getType() == MSG_ADD_CLIENT)
	{
		answer = executeAddClient((AddClientMessage*)msg);
		update_client_list = true;
	}

	// Check whether the message was addressed to us
	else if (msg->getServerDest() != name)
	{
		log.print(LOG_ERROR, "[!] Message addressed to %s, but I'm %s\n",
		          msg->getServerDest().c_str(), name.c_str());

		answer = new ErrorMessage("Wrong_destination");
	}
	else
	{
		switch (msg->getType())
		{
		case MSG_SEND_MESSAGE:
			answer = executeSendMessage((SendMessage *)msg);
			break;

		default:
			log.print(LOG_INFO, "[#] Unknown message type\n");
			break;
		}
	}

	if (answer != NULL)
	{
		ret = answer->Reply(skt);
		delete answer;
		log.print(LOG_DEBUG, "[ ] Replied to %s message\n", msg->typeToString().c_str());
	}

	if (close(skt) < 0)
		log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
		          strerror(errno));
	else
		log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

	// Update client list, if necessary
	if (update_client_list == true)
		updateClientListAdd(msg);

	return ret;
}

Message* CimabueServer::executeSendMessage(SendMessage *msg)
{
	/* 1) Check if destination client is registered
	 * 2) Get destination IP:port
	 * 3) Send message to destination
	 * 4) Send reply to source
	 */

	log.print(LOG_DEBUG, "CimabueServer::executeSendMessage(%s)\n",
	          msg->getClientDest().c_str());

	client_t dest_client;

	if (getClient(msg->getClientDest(), &dest_client))
	{
		Message *forward_msg = new Message();

		// TODO: Do I really need to copy it?
		msg->copyTo(*forward_msg);
		forward_msg->setServerSource(name);
		forward_msg->setServerDest(MSG_VOID);

		Message *reply = ((SendMessage*)forward_msg)->Send(dest_client.ip, dest_client.port);

		return reply;
	}
	else
	{
		log.print(LOG_ERROR, "[!] Destination %s not found!\n",
		          msg->getClientDest().c_str());

		return new ErrorMessage("Destination_not_found");
	}
}

Message* CimabueServer::executeAddClient(AddClientMessage *msg)
{
	client_t new_client;
	string clientName = msg->getClientSource();

	new_client.nickname = msg->getClientNickname();
	new_client.ip = msg->getClientIP();
	new_client.port = msg->getClientPort();

	// Add client to list
	pthread_mutex_lock(&clientList_mutex);
	clientList.insert(pair<string, client_t> (msg->getClientSource(), new_client));
	pthread_mutex_unlock(&clientList_mutex);

	log.print(LOG_DEBUG, "[+] Added client: \"%s\" (%s:%d)\n",
	          new_client.nickname.c_str(), new_client.ip.c_str(), new_client.port);

	// Reply to Client
	return new Message(MSG_ADD_CLIENT, MSG_VOID, msg->getClientSource(), name,
	                   MSG_VOID, msg->getData());
}

void CimabueServer::updateClientListAdd(Message *msg)
{
	// TODO TODO TODO TODO TODO TODO TODO
	/*
	map<string, string>::iterator clientListIter;

	pthread_mutex_lock(&clientNameList_mutex);

	for (clientListIter = clientNameToIPMap.begin(); clientListIter
	        != clientNameToIPMap.end(); clientListIter++)
	{
		log.print(LOG_DEBUG, "Updating %s list...\n",
		          clientListIter->first.c_str());

		if (clientListIter->first != msg->getClientSource())
		{
			// Send clientNickname:clientName
			string entry = "1:";
			entry += parseNickname(msg->getData());
			entry += ":";
			entry += msg->getClientSource();

			Message update_msg(MSG_UPDATE_ADD_CLIENTS, MSG_VOID,
			                   clientListIter->first, name, MSG_VOID, entry);

			string update_ip = clientListIter->second;
			unsigned int update_port;

			if (parseAddress(&update_ip, &update_port))
				update_msg.Send(update_ip, update_port);
		}
		else
		{
			char num_clients[16];
			memset(num_clients, 0, 16);
			sprintf(num_clients, "%d", (int) clientNicknameToName.size());
			string entry = num_clients;

			map<string, string>::iterator iter;

			for (iter = clientNicknameToName.begin(); iter
			        != clientNicknameToName.end(); iter++)
			{
				entry += ":";
				entry += iter->second;
				entry += ":";
				entry += iter->first;
			}

			Message update_msg(MSG_UPDATE_ADD_CLIENTS, MSG_VOID,
			                   msg->getClientSource(), name, MSG_VOID, entry);

			string update_ip = clientListIter->second;
			unsigned int update_port;

			if (parseAddress(&update_ip, &update_port))
				update_msg.Send(update_ip, update_port);
		}
	}

	pthread_mutex_unlock(&clientNameList_mutex);
	*/
}

void CimabueServer::updateClientListRem(string node_name)
{
	// TODO TODO TODO TODO TODO TODO TODO
	/*
	map<string, string>::iterator clientListIter;

	pthread_mutex_lock(&clientList_mutex);

	for (clientListIter = clientList.begin(); clientListIter
	        != clientList.end(); clientListIter++)
	{
		if (clientListIter->first != node_name)
		{
			log.print(LOG_DEBUG, "Updating %s list...\n",
			          clientListIter->first.c_str());

			Message update_msg(MSG_UPDATE_REM_CLIENTS, MSG_VOID,
			                   clientListIter->first, name, MSG_VOID, node_name);

			string update_ip = clientListIter->second;
			unsigned int update_port;

			if (parseAddress(&update_ip, &update_port))
				update_msg.Send(update_ip, update_port);
		}
	}

	pthread_mutex_unlock(&clientList_mutex);
	*/
}

bool CimabueServer::getServer(string serverName, server_t *server)
{
	bool retval = false;

	pthread_mutex_lock(&serverList_mutex);
	map<string, server_t>::iterator iter;
	iter = serverList.find(serverName);

	if (iter != serverList.end())
	{
		*server = iter->second;
		retval = true;
	}
	pthread_mutex_unlock(&serverList_mutex);

	return retval;
}

void CimabueServer::removeClient(string clientName)
{
	pthread_mutex_lock(&clientList_mutex);
	clientList.erase(clientName);
	pthread_mutex_unlock(&clientList_mutex);
}

bool CimabueServer::haveClient(string clientName)
{
	if (clientList.find(clientName) != clientList.end())
		return true;
	else
		return false;
}

bool CimabueServer::getClient(string clientName, client_t *client)
{
	bool retval = false;

	pthread_mutex_lock(&clientList_mutex);
	map<string, client_t>::iterator iter;
	iter = clientList.find(clientName);

	if (iter != clientList.end())
	{
		*client = iter->second;
		retval = true;
	}
	pthread_mutex_unlock(&clientList_mutex);

	return retval;
}

server_role_t CimabueServer::getRole()
{
	return role;
}
