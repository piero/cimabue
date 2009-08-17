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
    pingProxy_is_running = false;
    ping_enabled = enablePing;

    log.print(LOG_DEBUG, "CimabueServer()\n");

    pthread_mutex_init(&proxyList_mutex, NULL);
    pthread_mutex_init(&clientList_mutex, NULL);
    pthread_mutex_init(&serverList_mutex, NULL);
    pthread_mutex_init(&proxyServerList_mutex, NULL);
    pthread_mutex_init(&proxyPingList_mutex, NULL);

    if (ping_enabled)
    {
        // Ping connected clients
        if (pthread_create(&pingProxy_tid, NULL, do_pingProxy, this) != 0)
        {
            log.print(LOG_ERROR, "[!] Error creating pinging thread\n");
        }
    }
}

CimabueServer::~CimabueServer()
{
    if (pingProxy_is_running)
    {
        // Join pingProxy thread
        if (pthread_join(pingProxy_tid, NULL) != 0)
            log.print(LOG_ERROR, "[!] Error joining proxy_ping thread\n");
    }

    pthread_mutex_destroy(&proxyList_mutex);
    pthread_mutex_destroy(&clientList_mutex);
    pthread_mutex_destroy(&serverList_mutex);
    pthread_mutex_destroy(&proxyServerList_mutex);
    pthread_mutex_destroy(&proxyPingList_mutex);

    log.print(LOG_DEBUG, "[x] Server\n");
}


void* CimabueServer::do_pingProxy(void *myself)
{
    CimabueServer *me = (CimabueServer*) myself;
    me->pingProxy_is_running = true;

    while (!me->die)
    {
        // Send a PING_PROXY message to each connected client
        map<string, string>::iterator proxy_iter;
        pthread_mutex_lock(&me->proxyList_mutex);

        for (proxy_iter = me->proxyList.begin(); proxy_iter != me->proxyList.end(); proxy_iter++)
        {
            me->log.print(LOG_DEBUG, "[ ] Pinging %s...\n", proxy_iter->first.c_str());

            Message ping(MSG_PING_PROXY,
                         MSG_VOID, MSG_VOID,
                         MSG_VOID, proxy_iter->first,
                         me->name, MSG_VOID,
                         MSG_VOID);

            Message *pong = ping.Send(proxy_iter->second, NODE_PORT_PROXY_UP);

            if (!pong->isErrorMessage())
            {
                // Update timestamp
                map<string, timestamp_t>::iterator ping_iter;
                pthread_mutex_lock(&me->proxyPingList_mutex);
                ping_iter = me->proxyPingList.find(proxy_iter->first);
                ping_iter->second = me->getTimestamp();

                me->log.print(LOG_DEBUG, "[ ] Updated timestamp: %s - %ld\n",
                              ping_iter->first.c_str(), ping_iter->second);
                pthread_mutex_unlock(&me->proxyPingList_mutex);
            }
            else
                me->log.print(LOG_ERROR, "[!] Error pinging %s: %s\n",
                              proxy_iter->first.c_str(),
                              ((ErrorMessage*) pong)->getErrorMessage().c_str());

            delete pong;
        }
        pthread_mutex_unlock(&me->proxyList_mutex);

        // Delete timed-out clients
        map<string, timestamp_t>::iterator ping_iter;
        pthread_mutex_lock(&me->proxyPingList_mutex);

        for (ping_iter = me->proxyPingList.begin(); ping_iter
                != me->proxyPingList.end(); ping_iter++)
        {
            timestamp_t now = me->getTimestamp();
            timestamp_t delta = now - ping_iter->second;

            if (delta > 5 * 1E9)
            {
                me->log.print(LOG_INFO, "[@] Proxy %s timed-out (%lld)\n",
                              ping_iter->first.c_str(), delta);

                // Remove proxy from proxyList and proxyPingList
                pthread_mutex_unlock(&me->proxyPingList_mutex);
                me->removeProxy(ping_iter->first);
                pthread_mutex_lock(&me->proxyPingList_mutex);
            }
        }
        pthread_mutex_unlock(&me->proxyPingList_mutex);

        sleep(5);
    }

    me->pingProxy_is_running = false;
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

    log.print(LOG_INFO, "[ ] Processing message from a PROXY\n");

    // A new proxy doesn't know our name, but only our IP address
    if (msg->getType() == MSG_ADD_PROXY)
    {
        answer = executeAddProxy(msg);
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

        case MSG_REM_PROXY:
            answer = executeRemProxy(msg);
            break;

        case MSG_ADD_CLIENT:
            answer = executeAddClient(msg);
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

    return ret;
}

Message* CimabueServer::executeSendMessage(Message *msg)
{
    // Retrieve the proxy connected to the destination
    string proxy = getProxyWithClient(msg->getClientDest());

    if (proxy == MSG_VOID)
    {
        log.print(LOG_ERROR, "[!] Cannot find any Proxy with client %s\n",
                  msg->getClientDest().c_str());

        ErrorMessage *reply = new ErrorMessage();
        reply->setErrorMessage("Proxy not found");
        return reply;
    }

    log.print(LOG_DEBUG, "[ ] Client %s is connected to Proxy %s\n",
              msg->getClientSource().c_str(), proxy.c_str());

    string server = whoHasProxy(proxy);

    if (server == MSG_VOID)
    {
        log.print(LOG_ERROR, "[!] Cannot find any Server with proxy %s\n",
                  proxy.c_str());

        ErrorMessage *reply = new ErrorMessage();
        reply->setErrorMessage("Server not found");
        return reply;
    }

    if (server == name)
    {
        // Forward message to local proxy
        log.print(LOG_DEBUG, "[ ] Forward message to local proxy (%s)\n",
                  proxy.c_str());

        Message *forward_msg = new Message();
        msg->copy(*forward_msg);
        forward_msg->setServerSource(name);
        forward_msg->setProxyDest(proxy);

        Message *reply = forward_msg->Send(getProxyAddress(proxy), NODE_PORT_PROXY_UP);

        return reply;
    }
    else
    {
        // Forward message to remote server
        log.print(LOG_DEBUG, "[ ] Forward message to remote server (%s)\n",
                  server.c_str());

        Message *reply = new Message();
        msg->copy(*reply);
        reply->setServerSource(name);
        reply->setServerDest(server);
        return reply;
    }
}

Message* CimabueServer::executeAddProxy(Message *msg)
{
    // Add Proxy to related lists
    pthread_mutex_lock(&proxyList_mutex);
    proxyList.insert(pair<string, string> (msg->getProxySource(),
                                           msg->getData()));
    pthread_mutex_unlock(&proxyList_mutex);

    pthread_mutex_lock(&proxyServerList_mutex);
    proxyServerList.insert(pair<string, string> (msg->getProxySource(), name));
    pthread_mutex_unlock(&proxyServerList_mutex);

    pthread_mutex_lock(&proxyPingList_mutex);
    proxyPingList.insert(pair<string, timestamp_t> (msg->getProxySource(), getTimestamp()));

    log.print(LOG_INFO, "[+] Added Proxy %s --> %s (%d proxies)\n",
              msg->getProxySource().c_str(), msg->getData().c_str(), proxyPingList.size());
    pthread_mutex_unlock(&proxyPingList_mutex);

    Message *reply = new Message(MSG_ADD_PROXY,
                                 MSG_VOID, MSG_VOID,
                                 MSG_VOID, msg->getProxySource(),
                                 name, MSG_VOID,
                                 MSG_VOID);
    return reply;
}

Message* CimabueServer::executeRemProxy(Message *msg)
{
    Message *reply = NULL;

    removeProxy(msg->getProxySource());

    reply = new Message();
    msg->copy(*reply);

    return reply;
}

Message* CimabueServer::executeAddClient(Message *msg)
{
    Message *reply = NULL;

    // Add client to list
    pthread_mutex_lock(&clientList_mutex);
    clientList.insert(pair<string, string> (msg->getData(),
                                            msg->getProxySource()));

    log.print(LOG_INFO, "[+] Added Client %s --> %s (%d clients)\n",
              msg->getData().c_str(), msg->getProxySource().c_str(),
              clientList.size());

    pthread_mutex_unlock(&clientList_mutex);

    // Reply to Proxy
    reply = new Message(MSG_ADD_CLIENT,
                        MSG_VOID, MSG_VOID,
                        MSG_VOID, msg->getProxySource(),
                        name, MSG_VOID,
                        msg->getData());

    return reply;
}

Message* CimabueServer::executeRemClient(Message *msg)
{
    Message *reply = NULL;

    // Remove client from list
    pthread_mutex_lock(&clientList_mutex);
    clientList.erase(msg->getData());
    pthread_mutex_unlock(&clientList_mutex);

    reply = new Message();
    return reply;
}

bool CimabueServer::haveProxy(string const proxyName)
{
    bool retval = false;

    pthread_mutex_lock(&proxyList_mutex);
    map<string, string>::iterator iter;
    iter = proxyList.find(proxyName);

    if (iter != proxyList.end())
        retval = true;
    pthread_mutex_unlock(&proxyList_mutex);

    return retval;
}

string CimabueServer::getProxyWithClient(string const clientName)
{
    string retval = MSG_VOID;

    pthread_mutex_lock(&clientList_mutex);
    map<string, string>::iterator iter;
    iter = clientList.find(clientName);

    if (iter != clientList.end())
        retval = iter->second;
    pthread_mutex_unlock(&clientList_mutex);

    return retval;
}

string CimabueServer::getServerAddress(string const serverName)
{
    string retval = MSG_VOID;

    pthread_mutex_lock(&serverList_mutex);
    map<string, string>::iterator iter;
    iter = serverList.find(serverName);

    if (iter != serverList.end())
        retval = iter->second;
    pthread_mutex_unlock(&serverList_mutex);

    return retval;
}

string CimabueServer::whoHasProxy(string const proxyName)
{
    string retval = MSG_VOID;

    pthread_mutex_lock(&proxyServerList_mutex);
    map<string, string>::iterator iter;
    iter = proxyServerList.find(proxyName);

    if (iter != proxyServerList.end())
        retval = iter->second;
    pthread_mutex_unlock(&proxyServerList_mutex);

    return retval;
}

string CimabueServer::getProxyAddress(string const proxyName)
{
    string retval = MSG_VOID;

    pthread_mutex_lock(&proxyList_mutex);
    map<string, string>::iterator iter;
    iter = proxyList.find(proxyName);

    if (iter != proxyList.end())
        retval = iter->second;
    pthread_mutex_unlock(&proxyList_mutex);

    return retval;
}

void CimabueServer::removeProxy(std::string proxyName)
{
	// Remove the Proxy from all related lists

    pthread_mutex_lock(&proxyList_mutex);
    proxyList.erase(proxyName);
    pthread_mutex_unlock(&proxyList_mutex);

    pthread_mutex_lock(&proxyServerList_mutex);
    proxyServerList.erase(proxyName);
    pthread_mutex_unlock(&proxyServerList_mutex);

    pthread_mutex_lock(&proxyPingList_mutex);
    proxyPingList.erase(proxyName);
    pthread_mutex_unlock(&proxyPingList_mutex);
}
