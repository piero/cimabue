/*
* StateManager.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#include "client_states/StateInit.h"
#include "CimabueClient.h"
#include <list>

class Observer;

class StateManager
{
public:
    StateManager();
    virtual ~StateManager();

    void setState(ClientState *newState);
    ClientState* getState();

    void addView(Observer *view);
    void remView(Observer *view);
    void updateViews(Event event);

    void setServerIP(std::string ip);
    void setServerPort(unsigned short port);

    unsigned short getClientPort();
    std::string getServerIP();
    unsigned short getServerPort();

    std::string getNickname();

    void connectToServer();
    bool isConnected();

    void handleInput();
    void init(std::string nick, unsigned int localPort, std::string serverIP, unsigned int serverPort);

    CimabueClient* getConnector();

private:
    ClientState *current_state;
    CimabueClient *connector;
    std::list<Observer*> views;

    std::string nickname;
    unsigned short client_port;
    std::string server_ip;
    unsigned short server_port;

    bool connected;

    Log log;
};


#endif /* STATEMANAGER_H_ */
