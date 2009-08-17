/*
* CimabueClient.cpp
*
*  Created on: May 4, 2009
*      Author: piero
*/

#include "CimabueClient.h"

using namespace std;

CimabueClient::CimabueClient(unsigned short localPort, string serverIP, unsigned short serverPort) :
		Node(localPort + 1, localPort)
{
	connectedToServer = false;
	server_ip = serverIP;
	server_port = serverPort;
	nickname = "";

	// Connect to Server
	connectToServer(server_ip);
}

CimabueClient::~CimabueClient()
{
	die = true;
}

void CimabueClient::connectToServer(string serverIP)
{
	log.print(LOG_INFO, "[CimabueClient] Trying to connect to Server %s...\n", serverIP.c_str());

	// Concatenate NICKNAME ## IP ## PORT
	char nickname_ip_port[128];
	memset(nickname_ip_port, 0, 128);
	sprintf(nickname_ip_port, "%s:%s:%d", nickname.c_str(), ip.c_str(), portList[DOWN_PORT]);

	// Send message to server
	Message connectMe(MSG_ADD_CLIENT,
	                  name, MSG_VOID,
	                  MSG_VOID, MSG_VOID,
	                  MSG_VOID, MSG_VOID,
	                  nickname_ip_port);

	while (!connectedToServer)
	{
		Message *ret = connectMe.Send(serverIP, server_port);

		if (!ret->isErrorMessage())
		{
			proxy = ret->getProxySource();
			connectedToServer = true;
		}
		else
		{
			log.print(LOG_ERROR, "[!] Error connecting to Proxy: %s\n",
			          ((ErrorMessage*) ret)->getErrorMessage().c_str());

			// Dammit! Retry...
			sleep(2);
		}

		delete ret;
	}

	log.print(LOG_INFO, "[ ] Connected to Proxy: %s\n", proxy.c_str());
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

	string data_to_send = nickname;
	data_to_send += DATA_SEPARATOR;
	data_to_send += content;

	Message msg(MSG_SEND_MESSAGE,
	            name, dest,
	            MSG_VOID, proxy,
	            MSG_VOID, MSG_VOID,
	            data_to_send);

	Message *reply = msg.Send(server_ip, server_port);

	if (reply->isErrorMessage())
	{
		log.print(LOG_ERROR, "[!] Error connecting to Proxy: %s",
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
			log.print(LOG_DEBUG, "[ ] Replied to PING request\n");
		}
		break;

	case MSG_SEND_MESSAGE:
		{
			string sender_nick;
			string msg_content;

			extractNicknameAndData(msg->getData(), sender_nick, msg_content);

			log.print(LOG_WARNING, "[*] Received message from %s:\n%s\n",
			          sender_nick.c_str(), msg_content.c_str());

			Message ack;
			ack.Reply(skt);
			log.print(LOG_DEBUG, "[ ] Replied to SEND_MESSAGE request\n");
		}
		break;

	default:
		log.print(LOG_INFO, "[#] Unknown message type (down)\n");
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
			// We've been added to a Proxy
			proxy = msg->getProxySource();
			connectedToServer = true;

			log.print(LOG_INFO, "[ ] Connected to Proxy (%s)\n", proxy.c_str());
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
