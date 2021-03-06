/*
* CimabueClient.cpp
*
*  Created on: May 4, 2009
*      Author: piero
*/

#include "CimabueClient.h"
#include "StateManager.h"

using namespace std;

CimabueClient::CimabueClient(StateManager *caller) :
		Node(caller->getClientPort() + 1, caller->getClientPort())
{
	manager = caller;

	connectedToServer = false;
	server_ip = manager->getServerIP();
	server_port = manager->getServerPort();
	nickname = manager->getNickname();

	// Connect to Server
	connectToServer(server_ip);
}

CimabueClient::~CimabueClient()
{
	die = true;
}

void CimabueClient::connectToServer(string serverIP)
{
	log.print(LOG_INFO, "[ ] Trying to connect to Server %s...\n", serverIP.c_str());

	// Notify views
	EventConnecting event_connecting(serverIP, server_port);
	manager->updateViews(event_connecting);

	// Concatenate NICKNAME ## IP ## PORT
	char nickname_ip_port[128];
	memset(nickname_ip_port, 0, 128);
	sprintf(nickname_ip_port, "%s:%s:%d", nickname.c_str(), ip.c_str(), portList[DOWN_PORT]);

	// Send message to Server
	Message connectMe(MSG_ADD_CLIENT,
	                  name, MSG_VOID,
	                  MSG_VOID, MSG_VOID,
	                  nickname_ip_port);

	while (!connectedToServer)
	{
		Message *ret = connectMe.Send(serverIP, server_port);

		if (!ret->isErrorMessage())
		{
			server = ret->getServerSource();
			connectedToServer = true;
		}
		else
		{
			log.print(LOG_ERROR, "[!] Error connecting to Server: %s\n",
			          ((ErrorMessage*) ret)->getErrorMessage().c_str());

			// Dammit! Retry...
			sleep(2);
		}

		delete ret;
	}

	log.print(LOG_INFO, "[ ] Connected to Server: %s\n", server.c_str());

	// Notify views
	EventConnected event_connected(serverIP, server_port);
	manager->updateViews(event_connected);
}

void CimabueClient::setProxyIP(string ip)
{
	server_ip = ip;
}

string CimabueClient::getProxyIP()
{
	return server_ip;
}

int CimabueClient::sendMessage(string dest, string content)
{
	int ret = RET_ERROR;

	map<string, string>::iterator iter;

	iter = clientNickToNameMap.find(dest);
	if (iter == clientNickToNameMap.end())
	{
		log.print(LOG_ERROR, "[!] Couldn't find \"%s\" in client list\n", dest.c_str());
		return ret;
	}

	dest = iter->second;

	string data_to_send = nickname;
	data_to_send += DATA_SEPARATOR;
	data_to_send += content;

	Message msg(MSG_SEND_MESSAGE,
	            name, dest,
	            MSG_VOID, server,
	            data_to_send);

	Message *reply = msg.Send(server_ip, server_port);

	if (reply->isErrorMessage())
	{
		log.print(LOG_ERROR, "[!] Error connecting to Server: %s",
		          ((ErrorMessage*) ret)->getErrorMessage().c_str());
	}
	else
		ret = RET_SUCCESS;

	delete reply;
	return ret;
}

int CimabueClient::processDownMessage(Message *msg, int skt)
{
	// Check whether the message was addressed to us
	if (msg->getClientDest() != name)
	{
		log.print(LOG_ERROR, "[!] Message addressed to %s, but I'm %s\n",
		          msg->getClientDest().c_str(), name.c_str());

		if (close(skt) < 0)
			log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
			          strerror(errno));
		else
			log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

		return NODE_RET_DESTINATION_IS_NOT_ME;
	}

	switch (msg->getType())
	{
	case MSG_PING_CLIENT:
		{
			// Send ping reply
			Message pong;
			pong.Reply(skt);
			log.print(LOG_PARANOID, "[ ] Replied to PING request\n");
		}
		break;

	case MSG_SEND_MESSAGE:
		{
			string sender_nick;
			string msg_content;

			extractNicknameAndData(msg->getData(), sender_nick, msg_content);

			log.print(LOG_PARANOID, "[*] Received message from %s:\n%s\n",
			          sender_nick.c_str(), msg_content.c_str());

			Message ack;
			ack.Reply(skt);
			log.print(LOG_DEBUG, "[ ] Replied to SEND_MESSAGE request\n");

			// Notify views
			EventMessage event(sender_nick, msg_content);
			manager->updateViews(event);
		}
		break;

	case MSG_UPDATE_ADD_CLIENTS:
		{
			log.print(LOG_PARANOID, "[ ] Received MSG_UPDATE_ADD_CLIENTS message\n");

			// Retrieve the number of Clients
			size_t first_separator = msg->getData().find(":");
			string client_number_s = msg->getData().substr(0, first_separator);
			unsigned int client_number = atoi(client_number_s.c_str());

			log.print(LOG_DEBUG, "Adding %d client/s...\n", client_number);

			// Strip # information from data
			string data = msg->getData().substr(first_separator + 1);

			for (unsigned int i = 0; i < client_number; ++i)
			{
				pair<string, string> new_client = extractNewClient(data);

				clientNickToNameMap.insert(pair<string, string>(new_client.first, new_client.second));

				log.print(LOG_DEBUG, "[ ] Added (%s, %s) to client list\n",
				          new_client.first.c_str(), new_client.second.c_str());

				// Notify views
				EventUpdateAdd event(new_client.first);
				manager->updateViews(event);
			}

			Message ack;
			ack.Reply(skt);
			log.print(LOG_DEBUG, "[ ] Replied to MSG_UPDATE_ADD_CLIENTS request\n");
		}
		break;

	case MSG_UPDATE_REM_CLIENTS:
		{
			log.print(LOG_PARANOID, "[ ] Received MSG_UPDATE_REM_CLIENTS message: %s\n", msg->getData().c_str());

			map<string, string>::iterator iter;

			for (iter = clientNickToNameMap.begin();
			        iter != clientNickToNameMap.end();
			        iter++)
			{
				if (iter->second == msg->getData())
				{
					log.print(LOG_DEBUG, "[x] Removing %s\n", iter->second.c_str());

					string nick = iter->first;
					clientNickToNameMap.erase(iter);

					// Update views
					EventUpdateRem event(nick);
					manager->updateViews(event);

					break;
				}
			}
		}
		break;

	default:
		log.print(LOG_ERROR, "[#] Unknown message type (down)\n");
		break;
	}

	if (close(skt) < 0)
		log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
		          strerror(errno));
	else
		log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

	return RET_SUCCESS;
}

int CimabueClient::processUpMessage(Message *msg, int skt)
{
	// Check whether the message was addressed to us
	if (msg->getClientDest() != name)
	{
		log.print(LOG_ERROR, "[!] Message addressed to %s, but I'm %s\n",
		          msg->getClientDest().c_str(), name.c_str());

		if (close(skt) < 0)
			log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
			          strerror(errno));
		else
			log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

		return NODE_RET_DESTINATION_IS_NOT_ME;
	}

	switch (msg->getType())
	{
	case MSG_ADD_CLIENT:
		{
			// We've been added to a Server
			server = msg->getServerSource();
			connectedToServer = true;

			log.print(LOG_INFO, "[ ] Connected to Server (%s)\n", server.c_str());
		}
		break;

	default:
		log.print(LOG_INFO, "[#] Unknown message type (up)\n");
		break;
	}

	if (close(skt) < 0)
		log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
		          strerror(errno));
	else
		log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

	return RET_SUCCESS;
}

string CimabueClient::getNickname()
{
	return nickname;
}

void CimabueClient::extractNicknameAndData(string s, string &nick, string &data)
{
	string::size_type lastPos = s.find_first_not_of(DATA_SEPARATOR, 0);
	string::size_type pos = s.find_first_of(DATA_SEPARATOR, 0);

	list<string> tokens;

	while (string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(s.substr(lastPos, pos - lastPos));
		lastPos = s.find_first_not_of(DATA_SEPARATOR, pos);
		pos = s.find_first_of(DATA_SEPARATOR, lastPos);
	}

	nick = tokens.front();
	tokens.pop_front();
	data = tokens.front();
}

pair<string, string> CimabueClient::extractNewClient(std::string &data)
{
	string nick, name;
	size_t first_separator = data.find(":");

	nick = data.substr(0, first_separator);

	size_t second_separator = data.find(":", first_separator + 1);
	if (second_separator != string::npos)
	{
		name = data.substr(first_separator + 1, second_separator - first_separator - 1);
		data = data.substr(second_separator + 1);
	}
	else
	{
		// Last entry
		name = data.substr(first_separator + 1);
		data = MSG_VOID;
	}

	return pair<string, string>(nick, name);
}
