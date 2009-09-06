/*
* ServerProxy.cpp
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#include "ServerProxy.h"
#include "CimabueClient.h"

using namespace std;


ServerProxy::ServerProxy(CimabueClient *caller, unsigned short port, long select_timeout)
        : ActiveNode(port, select_timeout)
{
    parent = caller;
    connected = false;
}

ServerProxy::~ServerProxy()
{}

bool ServerProxy::isConnected()
{
    return connected;
}

string ServerProxy::getServerName()
{
    return server_name;
}

void ServerProxy::setServerName(std::string serverName)
{
    server_name = serverName;
}

string ServerProxy::getServerIP()
{
    return server_ip;
}

void ServerProxy::setServerIP(std::string ip)
{
    server_ip = ip;
}

unsigned short ServerProxy::getServerPort()
{
    return server_port;
}

void ServerProxy::setServerPort(unsigned short port)
{
    server_port = port;
}

int ServerProxy::connect(string serverIP, unsigned short serverPort, string &outcome)
{
    server_ip = serverIP;
    server_port = serverPort;

    AddClientMessage connectMe(name, parent->getNickname(), ip, listenPort);

    log.print(LOG_DEBUG, "[ ] Connecting to Server (%s:%d)...\n", server_ip.c_str(), server_port);
    Message *ret = connectMe.Send(server_ip, server_port);

    if (!ret->isErrorMessage())
    {
        outcome = "SUCCESS";
        connected = true;
    }
    else
    {
        outcome = ((ErrorMessage*)ret)->getErrorMessage();

        log.print(LOG_ERROR, "[!] Error connecting to Server (%s:%d): %s\n",
                          server_ip.c_str(), server_port, outcome.c_str());
    }

    delete ret;

    if (connected)
    	return RET_SUCCESS;
    else
    	return RET_ERROR;
}

int ServerProxy::sendMessage(string dest, string content)
{
    int ret = RET_ERROR;

    Message msg(MSG_SEND_MESSAGE,
                name, dest,
                MSG_VOID, server_name,
                content);

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

int ServerProxy::processMessage(Message *msg, int skt)
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

        /* TODO TODO TODO TODO TODO TODO TODO TODO
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
        */

        /* TODO TODO TODO TODO TODO TODO TODO
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
        */

        /* TODO TODO TODO TODO TODO TODO TODO TODO
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
        */

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
