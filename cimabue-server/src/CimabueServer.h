/*
* Server.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef CIMABUESERVER_H_
#define CIMABUESERVER_H_

#include <nodes/Node.h>

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

    void updateClientList(Message *msg);

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

    // Association: Client Nickname -> ClientName
    std::map<std::string, std::string> clientNicknameToName;
    pthread_mutex_t clientNicknameList_mutex;

    /*
     * Reminder: The association Client Nickname -> ClientName is only used by Clients,
     * 			 because the Server doesn't care about nicknames:  they are not unique
     * 			 and should be ignored at this level.
     * What happens is that the Server sends ClientNickName -> ClientName to clients
     * just to update their list. A user chooses the destination nickname and the Client
     * translates it into the corresponding ClientName, which is sent to the Server.
     */

    // Association: ClientName -> Client IP address
    std::map<std::string, std::string> clientNameToIPMap;
    pthread_mutex_t clientNameList_mutex;

    // Association clientName --> last_ping_timestamp
    std::map<std::string, timestamp_t> clientPingList;
    pthread_mutex_t clientPingList_mutex;
};


#endif /* CIMABUESERVER_H_ */
