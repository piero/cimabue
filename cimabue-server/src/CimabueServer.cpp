/*
* Server.cpp
*
*  Created on: May 4, 2009
*      Author: piero
*/

#include "CimabueServer.h"

using namespace std;

CimabueServer::CimabueServer(unsigned short port, bool enablePing) :
        Node(port + 1, port)
{
    pingClient_is_running = false;
    ping_enabled = enablePing;

    log.print(LOG_DEBUG, "CimabueServer()\n");

    pthread_mutex_init(&serverList_mutex, NULL);
    pthread_mutex_init(&clientNameList_mutex, NULL);
    pthread_mutex_init(&clientPingList_mutex, NULL);
    pthread_mutex_init(&clientNicknameList_mutex, NULL);

    if (ping_enabled)
    {
        // Ping connected clients
        if (pthread_create(&pingClient_tid, NULL, do_pingClient, this) != 0)
        {
            log.print(LOG_ERROR, "[!] Error creating pinging thread\n");
        }
    }
}

CimabueServer::~CimabueServer()
{
    if (pingClient_is_running)
    {
        // Join pingProxy thread
        if (pthread_join(pingClient_tid, NULL) != 0)
            log.print(LOG_ERROR, "[!] Error joining proxy_ping thread\n");
    }

    pthread_mutex_destroy(&serverList_mutex);
    pthread_mutex_destroy(&clientNameList_mutex);
    pthread_mutex_destroy(&clientPingList_mutex);
    pthread_mutex_destroy(&clientNicknameList_mutex);

    log.print(LOG_DEBUG, "[x] Server\n");
}


void* CimabueServer::do_pingClient(void *myself)
{
    CimabueServer *me = (CimabueServer*) myself;
    me->pingClient_is_running = true;

    while (!me->die)
    {
        // Send a PING_PROXY message to each connected client
        map<string, string>::iterator client_iter;
        pthread_mutex_lock(&me->clientNameList_mutex);

        for (client_iter = me->clientNameToIPMap.begin(); client_iter != me->clientNameToIPMap.end(); client_iter++)
        {
            me->log.print(LOG_DEBUG, "[ ] Pinging %s...\n", client_iter->first.c_str());

            Message ping(MSG_PING_CLIENT,
                         MSG_VOID, client_iter->first,
                         me->name, MSG_VOID,
                         MSG_VOID);

            Message *pong = ping.Send(client_iter->second, NODE_PORT_CLIENT_UP);

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
                me->log.print(LOG_INFO, "[@] Proxy %s timed-out (%lld)\n",
                              ping_iter->first.c_str(), delta);

                // Remove proxy from proxyList and proxyPingList
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


int CimabueServer::processUpMessage(Message *msg, int skt)
{
    log.print(LOG_INFO, "[ ] Processing message from another SERVER\n");

    if (close(skt) < 0)
        log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
                  strerror(errno));
    else
        log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

    return RET_SUCCESS;
}

int CimabueServer::processDownMessage(Message *msg, int skt)
{
    int ret = NODE_RET_ERROR;
    Message *answer = NULL;

    log.print(LOG_INFO, "[ ] Processing message from a CLIENT\n");

    bool update_client_list = false;

    // A new client doesn't know our name, but only our IP address
    if (msg->getType() == MSG_ADD_CLIENT)
    {
        answer = executeAddClient(msg);
        update_client_list = true;
    }

    // Check whether the message was addressed to us
    else if (msg->getServerDest() != name)
    {
        log.print(LOG_ERROR, "[!] Message addressed to %s, but I'm %s\n",
                  msg->getServerDest().c_str(), name.c_str());

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

        case MSG_REM_CLIENT:
            answer = executeRemClient(msg);
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
        log.print(LOG_DEBUG, "[ ] Replied to %s message\n", msg->typeToString(msg->getType()).c_str());
    }

    if (close(skt) < 0)
        log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n", skt,
                  strerror(errno));
    else
        log.print(LOG_PARANOID, "[x] Socket %d\n", skt);

    if (update_client_list)
        updateClientList(msg);

    return ret;
}

Message* CimabueServer::executeSendMessage(Message *msg)
{
    /* 1) Check if destination client is registered
     * 2) Get destination IP:port
     * 3) Send message to destination
     * 4) Send reply to source
     */

    log.print(LOG_DEBUG, "CimabueServer::executeSendMessage(%s)\n", msg->getClientDest().c_str());

    if (haveClient(msg->getClientDest()))
    {
        string dest_nick;
        string dest_ip = getClientAddress(msg->getClientDest());
        unsigned dest_port;

        parseNicknameAndAddress(&dest_nick, &dest_ip, &dest_port);

        Message *forward_msg = new Message();

        // TODO: Do I really need to copy it?
        msg->copyTo(*forward_msg);
        forward_msg->setServerSource(name);
        forward_msg->setServerDest(MSG_VOID);

        Message *reply = forward_msg->Send(dest_ip, dest_port);

        return reply;
    }
    else
    {
        log.print(LOG_ERROR, "[!] Destination %s not found!\n", msg->getClientDest().c_str());

        return new ErrorMessage("DESTINATION NOT FOUND");
    }
}

Message* CimabueServer::executeAddClient(Message *msg)
{
    // Add client to list
    pthread_mutex_lock(&clientNameList_mutex);

    clientNameToIPMap.insert(pair<string, string> (
                                 msg->getClientSource(),
                                 msg->getData()));

    log.print(LOG_INFO, "[+] Added Client %s --> %s (%d clients)\n",
              msg->getClientSource().c_str(), msg->getData().c_str(),
              clientNameToIPMap.size());

    pthread_mutex_unlock(&clientNameList_mutex);

    // Add client nickname
    pthread_mutex_lock(&clientNicknameList_mutex);

    string nickname = parseNickname(msg->getData());
    clientNicknameToName.insert(pair<string, string>
                                (nickname, msg->getClientSource()));

    pthread_mutex_unlock(&clientNicknameList_mutex);


    // Reply to Client
    return new Message(MSG_ADD_CLIENT,
                       MSG_VOID, msg->getClientSource(),
                       name, MSG_VOID,
                       msg->getData());
}

void CimabueServer::updateClientList(Message *msg)
{
    map<string, string>::iterator clientListIter;

    pthread_mutex_lock(&clientNameList_mutex);

    for (clientListIter = clientNameToIPMap.begin();
            clientListIter != clientNameToIPMap.end();
            clientListIter++)
    {
        if (clientListIter->first != msg->getClientSource())
        {
            // Send clientNickname:clientName
            string entry = parseNickname(msg->getData());
            entry += ":";
            entry += msg->getClientSource();

            log.print(LOG_DEBUG, "[OLD] Updating %s: \"%s\"...\n",
                      clientListIter->first.c_str(), entry.c_str());

            Message update_msg(MSG_UPDATE_CLIENTS,
                               MSG_VOID, clientListIter->first,
                               name, MSG_VOID,
                               entry);

            string update_ip = clientListIter->second;
            unsigned int update_port;

            if (parseAddress(&update_ip, &update_port))
                update_msg.Send(update_ip, update_port);
        }
        else
        {
            log.print(LOG_DEBUG, "--> Updating new client...\n");

            // Send many messages! --> TODO: Find a better way
            map<string, string>::iterator iter;

            for (iter = clientNicknameToName.begin();
                    iter != clientNicknameToName.end();
                    iter++)
            {
                log.print(LOG_DEBUG, "[NEW] Updating %s: \"%s\"...\n",
                          iter->first.c_str(), iter->second.c_str());

                string entry = iter->first;
                entry += ":";
                entry += iter->second;

                Message update_msg(MSG_UPDATE_CLIENTS,
                                   MSG_VOID, msg->getClientSource(),
                                   name, MSG_VOID,
                                   entry);

                string update_ip = clientListIter->second;
                unsigned int update_port;

                if (parseAddress(&update_ip, &update_port))
                    update_msg.Send(update_ip, update_port);
            }
        }


    }

    pthread_mutex_unlock(&clientNameList_mutex);
}

Message* CimabueServer::executeRemClient(Message *msg)
{
    // Remove client from list
    pthread_mutex_lock(&clientNameList_mutex);
    clientNameToIPMap.erase(msg->getData());
    pthread_mutex_unlock(&clientNameList_mutex);

    pthread_mutex_lock(&clientNicknameList_mutex);
    // TODO: Remove client from nickname list
    pthread_mutex_unlock(&clientNicknameList_mutex);

    return new Message(MSG_REM_CLIENT,
                       MSG_VOID, msg->getClientSource(),
                       name, MSG_VOID,
                       msg->getData());
}

string CimabueServer::getServerAddress(string serverName)
{
    string retval = MSG_VOID;

    pthread_mutex_lock(&serverList_mutex);
    map<string, string>::iterator iter;
    iter = serverNameToIPMap.find(serverName);

    if (iter != serverNameToIPMap.end())
        retval = iter->second;
    pthread_mutex_unlock(&serverList_mutex);

    return retval;
}

void CimabueServer::removeClient(string clientName)
{
    // Remove the Client from all related lists

    pthread_mutex_lock(&clientNameList_mutex);
    clientNameToIPMap.erase(clientName);
    pthread_mutex_unlock(&clientNameList_mutex);

    pthread_mutex_lock(&clientPingList_mutex);
    clientPingList.erase(clientName);
    pthread_mutex_unlock(&clientPingList_mutex);
}

bool CimabueServer::haveClient(string clientName)
{
    if (clientNameToIPMap.find(clientName) != clientNameToIPMap.end())
        return true;
    else
        return false;
}

string CimabueServer::getClientAddress(string clientName)
{
    return clientNameToIPMap.find(clientName)->second;
}
