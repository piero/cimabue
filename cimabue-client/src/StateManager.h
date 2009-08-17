/*
* StateManager.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#include "client_states/StateInit.h"
#include "nodes/CimabueProxy.h"
#include <list>

class Observer;

class StateManager
{
public:
    StateManager(unsigned short local_port, std::string ip, unsigned short port);
    virtual ~StateManager();

    void setState(ClientState *newState);
    ClientState* getState();

    void addView(Observer *view);
    void remView(Observer *view);
    void updateViews();

    void setServerIP(std::string ip);
    void setServerPort(unsigned short port);

    std::string getServerIP();
    unsigned short getServerPort();

    void connectToServer();
    bool isConnected();

    void handleInput();

private:
    ClientState *current_state;
    CimabueProxy *proxy;
    std::list<Observer*> views;

    unsigned short client_port;
    std::string server_ip;
    unsigned short server_port;

    bool connected;

    Log log;
};


#endif /* STATEMANAGER_H_ */
