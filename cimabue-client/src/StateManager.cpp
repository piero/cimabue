/*
* StateManager.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "StateManager.h"
#include "view/Observer.h"

using namespace std;

StateManager::StateManager()
        : current_state(NULL),
        connector(NULL),
        connected(false)
{}

StateManager::~StateManager()
{
    // Empty views
    views.empty();
}

void StateManager::setState(ClientState *newState)
{
    if (current_state != NULL)
    {
        log.print(LOG_DEBUG, "[ ] %s --> ", current_state->getName().c_str());
        current_state->onExit();
    }

    current_state = newState;
    log.print(LOG_DEBUG, "%s\n", current_state->getName().c_str());

    current_state->onEntry();
}

ClientState* StateManager::getState()
{
    return current_state;
}

void StateManager::addView(Observer *view)
{
    if (view != NULL)
    {
        views.push_back(view);
        log.print(LOG_DEBUG, "[+] Added view 0x%x\n", view);
    }
}

void StateManager::remView(Observer *view)
{
    if (view != NULL)
    {
        list<Observer*>::iterator iter;

        for (iter = views.begin(); iter != views.end(); iter++)
            if (*iter == view)
            {
                views.erase(iter);
                log.print(LOG_DEBUG, "[-] Removed view 0x%x\n", view);
                break;
            }
    }
}

void StateManager::updateViews()
{
    list<Observer*>::iterator iter;

    for (iter = views.begin(); iter != views.end(); iter++)
        (*iter)->update();
}

void StateManager::setServerIP(string ip)
{
    server_ip = ip;
}

void StateManager::setServerPort(unsigned short port)
{
    server_port = port;
}

string StateManager::getServerIP()
{
    return server_ip;
}

unsigned short StateManager::getServerPort()
{
    return server_port;
}

bool StateManager::isConnected()
{
    return connected;
}

void StateManager::connectToServer()
{
    connector = new CimabueClient(nickname, client_port, server_ip, server_port);

    if (connector != NULL)
        connected = true;
}

void StateManager::handleInput()
{
    current_state->handleInput();
}

void StateManager::init(string nick, unsigned int localPort, string serverIP, unsigned int serverPort)
{
    log.print(LOG_DEBUG, "[ ] StateManager::init()\n");

    if (connected && connector != NULL)
    {
        delete connector;
        connected = false;
    }

    nickname = nick;
    client_port = localPort;
    server_ip = serverIP;
    server_port = serverPort;

    setState(new StateInit(this));
}

CimabueClient* StateManager::getConnector()
{
    return connector;
}
