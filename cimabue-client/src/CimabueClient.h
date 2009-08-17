/*
* CimabueClient.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef CIMABUECLIENT_H_
#define CIMABUECLIENT_H_

#include <nodes/Node.h>
#include "Event.h"

#define DATA_SEPARATOR	"@"


class StateManager;

class CimabueClient : public Node
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
    int processUpMessage(Message *msg, int skt);
    int processDownMessage(Message *msg, int skt);

    void extractNicknameAndData(std::string s, std::string &nick, std::string &data);

    void connectToServer(std::string proxyIP);

    bool connectedToServer;

    std::string nickname;
    std::string server;
    std::string server_ip;
    unsigned int server_port;

    StateManager *manager;
};


#endif /* CIMABUECLIENT_H_ */
