/*
* CimabueClient.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef CIMABUECLIENT_H_
#define CIMABUECLIENT_H_

#include <node/ActiveNode.h>
#include "ServerProxy.h"

#define DATA_SEPARATOR	"@"


class StateManager;

class CimabueClient
{
public:
    CimabueClient(StateManager *caller, unsigned short clientPort);

    virtual ~CimabueClient();

    int sendMessage(std::string dest, std::string content);
    std::string getNickname();

    void connectToServer(std::string serverIP, unsigned short serverPort);

private:
    void extractNicknameAndData(std::string s, std::string &nick, std::string &data);

    std::pair<std::string, std::string> extractNewClient(std::string &data);


    std::string nickname;
    StateManager *manager;

    // Association: Client Nickname --> Client Name
    // This is the list of connected Clients (apart from us)
    std::map<std::string, std::string> clientNickToNameMap;

    ServerProxy *server_proxy;
    Log log;
};


#endif /* CIMABUECLIENT_H_ */
