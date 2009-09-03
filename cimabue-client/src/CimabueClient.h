/*
* CimabueClient.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef CIMABUECLIENT_H_
#define CIMABUECLIENT_H_

#include <node/ActiveNode.h>
#include "ServerProxyNode.h"

#define DATA_SEPARATOR	"@"


class StateManager;

class CimabueClient : public ActiveNode
{
public:
    CimabueClient(StateManager *caller);

    virtual ~CimabueClient();

    int sendMessage(std::string dest, std::string content);

    void setProxyIP(std::string ip);
    std::string getProxyIP();

    std::string getNickname();

private:
    static void* do_listen(void *myself);
    int processMessage(Message *msg, int skt);

    void extractNicknameAndData(std::string s, std::string &nick, std::string &data);

    std::pair<std::string, std::string> extractNewClient(std::string &data);

    void connectToServer(std::string proxyIP);

    bool connectedToServer;

    std::string nickname;
    std::string server;
    std::string server_ip;
    unsigned int server_port;

    StateManager *manager;

    // Association: Client Nickname --> Client Name
    // This is the list of connected Clients (apart from us)
    std::map<std::string, std::string> clientNickToNameMap;

    ServerProxyNode *server;
};


#endif /* CIMABUECLIENT_H_ */
