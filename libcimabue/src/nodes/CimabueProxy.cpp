/*
* Client.cpp
*
*  Created on: May 4, 2009
*      Author: piero
*/

#include "CimabueProxy.h"

using namespace std;

CimabueProxy::CimabueProxy(unsigned short localPort, std::string serverAddress, unsigned short serverPort, bool enablePing, bool enableServerLookup) :
        Node(localPort, localPort + 1)
{
    updateServer_is_running = false;
    pingClient_is_running = false;
    connectedToServer = false;
    currentServerIP = serverAddress;
    bestServerIP = MSG_VOID;
    ping_enabled = enablePing;
    server_lookup_enabled = enableServerLookup;
    server_port = serverPort;

    log.print(LOG_DEBUG, "[ ] Creating CimabueProxy (%d, %d) --> (%s:%d)...\n",
    		portList[UP_PORT], portList[DOWN_PORT], currentServerIP.c_str(), server_port);

    pthread_mutex_init(&clientList_mutex, NULL);
    pthread_mutex_init(&clientPingList_mutex, NULL);
    pthread_mutex_init(&clientNicknames_mutex, NULL);

    if (ping_enabled)
    {
        // Ping connected clients
        if (pthread_create(&pingClient_tid, NULL, do_pingClient, this) != 0)
        {
            log.print(LOG_ERROR, "[!] Error creating pinging thread\n");
        }
    }

	if (server_lookup_enabled)
	{
		// Create serverUpdate thread
		if (pthread_create(&updateServer_tid, NULL, do_updateServer, this) != 0)
		{
		    log.print(LOG_ERROR, "[!] Error creating serverUpdate thread\n");
		}

		// Connect to a Server
		while (bestServerIP == MSG_VOID)
		{
		    usleep(100);
		}

		currentServerIP = bestServerIP;
    }

    connectToServer(currentServerIP);
}

CimabueProxy::~CimabueProxy()
{
    die = true;

    if (updateServer_is_running)
    {
        // Join serverUpdate thread
        if (pthread_join(updateServer_tid, NULL) != 0)
            log.print(LOG_ERROR, "[!] Error joining serverUpdate thread\n");
    }

    if (pingClient_is_running)
    {
        // Join pingClient thread
        if (pthread_join(pingClient_tid, NULL) != 0)
            log.print(LOG_ERROR, "[!] Error joining client_ping thread\n");
    }

    pthread_mutex_destroy(&clientList_mutex);
    pthread_mutex_destroy(&clientPingList_mutex);
    pthread_mutex_destroy(&clientNicknames_mutex);

    log.print(LOG_PARANOID, "[x] Client\n");
}

void CimabueProxy::connectToServer(string serverIP)
{
    log.print(LOG_INFO, "[ ] Connecting to Server %s:%d...\n", serverIP.c_str(), server_port);

    // Send message to server
    Message connectMe(MSG_ADD_PROXY,
                      MSG_VOID, MSG_VOID,
                      name, MSG_VOID,
                      MSG_VOID, MSG_VOID,
                      ip);

    while (!connectedToServer)
    {
        Message *ret = connectMe.Send(serverIP, server_port);

        if (!ret->isErrorMessage())
        {
            // We've been added to a Server
            currentServer = ret->getServerSource();
            connectedToServer = true;
        }
        else
            sleep(2);

        delete ret;
    }

    log.print(LOG_INFO, "[ ] Connected to Server %s!!!\n",
              currentServer.c_str());
}

string CimabueProxy::getCurrentServer()
{
    return currentServer;
}

string CimabueProxy::getCurrentServerIP()
{
    return currentServerIP;
}

void CimabueProxy::setCurrentServerIP(string ip)
{
    currentServerIP = ip;
}

void* CimabueProxy::do_updateServer(void *myself)
{
    CimabueProxy *me = (CimabueProxy*) myself;
    me->updateServer_is_running = true;

    while (!me->die)
    {
        // TODO: get best server IP from OLSR list
        me->bestServerIP = "192.168.5.133";
        sleep(5);
    }

    me->updateServer_is_running = false;
    pthread_exit(0);
}

void* CimabueProxy::do_pingClient(void *myself)
{
    CimabueProxy *me = (CimabueProxy*) myself;
    me->pingClient_is_running = true;

    while (!me->die)
    {
        // Send a PING_CLIENT message to each connected client
        map<string, string>::iterator client_iter;
        pthread_mutex_lock(&me->clientList_mutex);

        for (client_iter = me->clientList.begin(); client_iter
                != me->clientList.end(); client_iter++)
        {
            me->log.print(LOG_DEBUG, "[ ] Pinging %s...\n", client_iter->first.c_str());

            Message ping(MSG_PING_CLIENT,
                         MSG_VOID, client_iter->first,
                         me->name, MSG_VOID,
                         MSG_VOID, MSG_VOID,
                         MSG_VOID);

            string client_ip = client_iter->second;
            unsigned int client_port;
            me->parseAddress(&(client_ip), &client_port);

            Message *pong = ping.Send(client_ip, client_port);

            if (!pong->isErrorMessage())
            {
                // Update timestamp
                map<string, timestamp_t>::iterator ping_iter;
                pthread_mutex_lock(&me->clientPingList_mutex);
                ping_iter = me->clientPingList.find(client_iter->first);
                ping_iter->second = me->getTimestamp();

                me->log.print(LOG_DEBUG, "[ ] Updated timestamp: %s - %ld\n",
                              ping_iter->first.c_str(), ping_iter->second);
                pthread_mutex_unlock(&me->clientPingList_mutex);
            }
            else
                me->log.print(LOG_ERROR, "[!] Error pinging %s: %s\n",
                              client_iter->first.c_str(),
                              ((ErrorMessage*) pong)->getErrorMessage().c_str());

            delete pong;
        }
        pthread_mutex_unlock(&me->clientList_mutex);

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

                // Notify the server
                Message
                toServer(MSG_REM_CLIENT, MSG_VOID, MSG_VOID, me->name,
                         MSG_VOID, MSG_VOID, me->currentServer,
                         ping_iter->first);

                Message *server_reply = toServer.Send(me->currentServerIP, NODE_PORT_SERVER_DOWN);

                if (server_reply->isErrorMessage())
                {
                    me->log.print(
                        LOG_ERROR,
                        "[!] Error removing client from server: %s\n",
                        ((ErrorMessage*) server_reply)->getErrorMessage().c_str());

                    // Since the Client has gone, there's nothing much to do...
                }

                delete server_reply;

                // Remove client from clientList and clientPingList
                pthread_mutex_unlock(&me->clientPingList_mutex);
                me->removeClient(ping_iter->first);
                pthread_mutex_lock(&me->clientPingList_mutex);
            }
        }
        pthread_mutex_unlock(&me->clientPingList_mutex);

        sleep(5);
    }

    me->pingClient_is_running = false;
    pthread_exit(0);
}

int CimabueProxy::processUpMessage(Message *msg, int skt)
{
    int ret = NODE_RET_ERROR;
    Message *answer = NULL;

    log.print(LOG_INFO, "[ ] Processing message from our SERVER\n");

    // Check whether the message was addressed to us
    if (msg->getProxyDest() != name)
    {
        log.print(LOG_ERROR, "[!] Message addressed to %s, but I'm %s\n",
                  msg->getProxyDest().c_str(), name.c_str());

        answer = new ErrorMessage();
        ((ErrorMessage*) answer)->setErrorMessage("Wrong destination");
    }
    else
    {
        switch (msg->getType())
        {
        case MSG_ADD_CLIENT:
            answer = executeAddClientAck(msg);
            break;

        case MSG_REM_CLIENT:
            answer = executeRemClient(msg);
            break;

        case MSG_ADD_PROXY:
            answer = executeAddProxyAck(msg);
            break;

        case MSG_SEND_MESSAGE:
            answer = executeSendMessage(msg);
            break;

        case MSG_PING_CLIENT:
            {
                log.print(LOG_INFO, "[ ] Processing MSG_PING_CLIENT...\n");

                if (msg->getClientSource() != MSG_VOID)
                {
                    map<string, timestamp_t>::iterator iter;

                    pthread_mutex_lock(&clientPingList_mutex);
                    iter = clientPingList.find(msg->getClientSource());

                    // Update timestamp information
                    if (iter != clientPingList.end())
                        iter->second = getTimestamp();
                    pthread_mutex_unlock(&clientPingList_mutex);
                }
            }
            break;

        case MSG_PING_PROXY:
            {
                Message pong;
                pong.Reply(skt);
                log.print(LOG_DEBUG, "[ ] Replied to PING request\n");
            }
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
    }

    if (close(skt) < 0)
        log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
                  strerror(errno));
    else
        log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

    return ret;
}

int CimabueProxy::processDownMessage(Message *msg, int skt)
{
    int ret = NODE_RET_ERROR;
    Message *answer = NULL;

    log.print(LOG_INFO, "[ ] Processing message from a CLIENT\n");

    if (msg->getType() == MSG_ADD_CLIENT)
    {
        answer = executeAddClient(msg);
    }

    // Check whether the message was addressed to us
    else if (msg->getProxyDest() != name)
    {
        log.print(LOG_ERROR, "[!] Message addressed to %s, but I'm %s\n",
                  msg->getProxyDest().c_str(), name.c_str());

        answer = new ErrorMessage();
        ((ErrorMessage*) answer)->setErrorMessage("Wrong destination");
    }
    else
    {
        switch (msg->getType())
        {
        case MSG_SEND_MESSAGE:
            answer = executeSendMessage(msg);
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
    }

    if (close(skt) < 0)
        log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
                  strerror(errno));
    else
        log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

    return RET_SUCCESS;
}

Message* CimabueProxy::executeAddClient(Message *msg)
{
    log.print(LOG_INFO, "[ ] ADDING CLIENT %s...\n", msg->getClientSource().c_str());

    Message *reply = NULL;

    if (msg->getClientSource() != MSG_VOID)
    {
        // Add client locally
        pthread_mutex_lock(&clientList_mutex);
        clientList.insert(pair<string, string> (msg->getClientSource(),
                                                msg->getData()));
        pthread_mutex_unlock(&clientList_mutex);

        pthread_mutex_lock(&clientPingList_mutex);
        clientPingList.insert(pair<string, timestamp_t> (
                                  msg->getClientSource(), getTimestamp()));

        log.print(LOG_INFO, "[+] Added CLIENT %s --> %s\n",
                  msg->getClientSource().c_str(), msg->getData().c_str());

        pthread_mutex_unlock(&clientPingList_mutex);

        // Update the Server
        Message *update_server = new Message(MSG_ADD_CLIENT,
                                             MSG_VOID, MSG_VOID,
                                             name, MSG_VOID,
                                             MSG_VOID, currentServer,
                                             msg->getClientSource());

        Message *server_reply = update_server->Send(currentServerIP,
                                NODE_PORT_SERVER_DOWN);
        delete update_server;

        // Parse Server reply
        if (server_reply->isErrorMessage())
        {
            log.print(LOG_ERROR, "[!] Error updating the Server: %s\n",
                      ((ErrorMessage*) server_reply)->getErrorMessage().c_str());

            reply = new ErrorMessage();
            ((ErrorMessage*) reply)->setErrorMessage(
                ((ErrorMessage*) server_reply)->getErrorMessage());
        }
        else
            reply = new Message(MSG_ADD_CLIENT,
                                MSG_VOID, msg->getClientSource(),
                                name, MSG_VOID,
                                MSG_VOID, MSG_VOID,
                                MSG_VOID);
    }

    else
    {
        reply = new ErrorMessage();
        ((ErrorMessage*) reply)->setErrorMessage("Unknown source");
    }

    return reply;
}

Message* CimabueProxy::executeAddClientAck(Message *msg)
{
    Message *reply = NULL;

    if (msg->getServerSource() == currentServer)
    {
        // Reply to Client
        log.print(LOG_DEBUG, "[ ] Replying to Client (%s)\n",
                  msg->getData().c_str());

        string clientAddress = getClientAddress(msg->getData());
        unsigned int clientPort = -1;

        if (clientAddress != MSG_VOID && parseAddress(&clientAddress, &clientPort))
        {
            reply = new Message(MSG_ADD_CLIENT,
                                MSG_VOID, msg->getData(),
                                name, MSG_VOID,
                                MSG_VOID, MSG_VOID,
                                MSG_VOID);
        }
    }

    return reply;
}

Message* CimabueProxy::executeRemClient(Message *msg)
{
    Message *reply = NULL;

    // TODO TODO TODO TODO TODO

    return reply;
}

Message* CimabueProxy::executeAddProxyAck(Message *msg)
{
    Message *reply = new Message();

    log.print(LOG_INFO, "[ ] Processing MSG_ADD_PROXY...\n");

    // We've been added to a Server
    currentServer = msg->getServerSource();
    connectedToServer = true;

    printf("[ ] Connected to Server: %s\n", currentServer.c_str());

    return reply;
}

Message* CimabueProxy::executeSendMessage(Message *msg)
{
    log.print(LOG_INFO, "[ ] Processing MSG_SEND_MESSAGE...\n");

    Message *reply = NULL;

    string destAddress = getClientAddress(msg->getClientDest());

    if (destAddress != MSG_VOID)
    {
        // If destination is our client, deliver the message
        unsigned int destPort;
        string name;
        //parseAddress(&destAddress, &destPort);
        parseNameAndAddress(&name, &destAddress, &destPort);

        log.print(LOG_DEBUG, "[ ] Message destination is our client (%s:%d)\n", destAddress.c_str(), destPort);

        reply = msg->Send(destAddress, destPort);
    }
    else
    {
        // Else forward the message to our Server
        log.print(LOG_DEBUG, "[ ] Forward message to Server\n");

        msg->setProxySource(name);
        msg->setServerDest(currentServer);
        reply = msg->Send(currentServerIP, NODE_PORT_SERVER_DOWN);
    }

    // TODO: Parse the reply

    return reply;
}

bool CimabueProxy::haveClient(string const clientName)
{
    bool retval = false;

    map<string, string>::iterator iter;
    iter = clientList.find(clientName);

    if (iter != clientList.end())
        retval = true;

    return retval;
}

string CimabueProxy::getClientAddress(string const clientName)
{
    string retval = MSG_VOID;

    map<string, string>::iterator iter;
    iter = clientList.find(clientName);

    if (iter != clientList.end())
        retval = iter->second;

    return retval;
}

void CimabueProxy::removeClient(std::string clientName)
{
    pthread_mutex_lock(&clientList_mutex);
    clientList.erase(clientName);
    pthread_mutex_unlock(&clientList_mutex);

    pthread_mutex_lock(&clientPingList_mutex);
    clientPingList.erase(clientName);
    pthread_mutex_unlock(&clientPingList_mutex);
}
