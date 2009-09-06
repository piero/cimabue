/*
* ServerProxy.h
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#ifndef SERVERPROXY_H_
#define SERVERPROXY_H_

#include <node/ActiveNode.h>

class CimabueClient;

class ServerProxy : public ActiveNode
{
public:
    ServerProxy(CimabueClient *caller, unsigned short port, long select_timeout = 5);
    virtual ~ServerProxy();

    int processMessage(Message *msg, int skt);

    bool isConnected();
    int connect(std::string serverIP, unsigned short serverPort, std::string &outcome);
    int sendMessage(std::string dest, std::string content);

    std::string getServerName();
    void setServerName(std::string serverName);

    std::string getServerIP();
    void setServerIP(std::string ip);

    unsigned short getServerPort();
    void setServerPort(unsigned short port);

protected:
    CimabueClient *parent;

    std::string server_name;
    std::string server_ip;
    unsigned short server_port;
    bool connected;
};


#endif /* SERVERPROXY_H_ */
