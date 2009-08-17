/*
* StateManager.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "StateManager.h"
#include "view/Observer.h"

using namespace std;

StateManager::StateManager(unsigned short local_port, std::string ip, unsigned short port)
        : proxy(NULL),
        client_port(local_port),
        server_ip(ip),
        server_port(port),
        connected(false)
{}

StateManager::~StateManager()
{
	// Empty views
	views.empty();
}

void StateManager::setState(ClientState *newState)
{
    current_state = newState;
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
	proxy = new CimabueProxy(client_port, server_ip, server_port);

	if (proxy != NULL)
		connected = true;
}

void StateManager::handleInput()
{
	current_state->handleInput();
}
