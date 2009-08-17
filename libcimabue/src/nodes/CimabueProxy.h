/*
* Client.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef CIMABUEPROXY_H_
#define CIMABUEPROXY_H_

#include "Node.h"

class CimabueProxy : public Node
{
public:
    CimabueProxy(unsigned short localPort, std::string serverAddress, unsigned short serverPort = NODE_PORT_SERVER_DOWN, bool enablePing = true, bool enableServerLookup = true);
    virtual ~CimabueProxy();

    std::string getCurrentServer();
    void setCurrentServerIP(std::string);

private:
    int processUpMessage(Message *msg, int skt);
    int processDownMessage(Message *msg, int skt);

    // Execute requests
    Message* executeSendMessage(Message *msg);
    Message* executeAddClient(Message *msg);
    Message* executeAddClientAck(Message *msg);
    Message* executeRemClient(Message *msg);
    Message* executeAddProxyAck(Message *msg);

    std::string getCurrentServerIP();
    void connectToServer(std::string serverIP);

    bool haveClient(std::string const clientName);	// Do I have the client in my list?
    std::string getClientAddress(std::string const clientName);	// Get IP:PORT of a given Client

    void removeClient(std::string clientName);

    // Periodically find the best server
    pthread_t updateServer_tid;
    bool updateServer_is_running;
    bool server_lookup_enabled;
    static void* do_updateServer(void *myself);

    // Ping connected clients
    pthread_t pingClient_tid;
    bool pingClient_is_running;
    bool ping_enabled;
    static void* do_pingClient(void *myself);

    unsigned short server_port;
    bool connectedToServer;

    std::string currentServer;
    std::string currentServerIP;
    std::string bestServerIP;

	// Association nickname --> "client_ip:client_port"
	std::map<std::string, std::string> clientNicknames;
	pthread_mutex_t clientNicknames_mutex;

    // Association clientName --> "client_ip:client_port"
    std::map<std::string, std::string> clientList;
    pthread_mutex_t clientList_mutex;

    // Association clientName --> last_ping_timestamp
    std::map<std::string, timestamp_t> clientPingList;
    pthread_mutex_t clientPingList_mutex;
};


#endif /* CIMABUEPROXY_H_ */
