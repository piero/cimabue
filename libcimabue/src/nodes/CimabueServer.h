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
    Message* executeAddClient(Message *msg);
    Message* executeRemClient(Message *msg);

    // Ping connected proxies
    pthread_t pingClient_tid;
    bool pingClient_is_running;
    static void* do_pingClient(void *myself);
    bool ping_enabled;

    // Do I have the Client in my list?
    bool haveClient(std::string clientName);

    std::string getClientAddress(std::string clientName);
    void removeClient(std::string clientName);

    // Get the IP of a given Server in serverList
    std::string getServerAddress(std::string const serverName);

    // Association: ServerName -> Server IP address
    std::map<std::string, std::string> serverNameToIPMap;
    pthread_mutex_t serverList_mutex;

    // Association: ClientName -> Client IP address
    std::map<std::string, std::string> clientNameToIPMap;
    pthread_mutex_t clientList_mutex;

    // Association clientName --> last_ping_timestamp
    std::map<std::string, timestamp_t> clientPingList;
    pthread_mutex_t clientPingList_mutex;
};


#endif /* CIMABUESERVER_H_ */
