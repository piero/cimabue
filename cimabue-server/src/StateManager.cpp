/*
* StateManager.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "StateManager.h"

using namespace std;

StateManager::StateManager(unsigned short local_port)
        : current_state(NULL),
        server(NULL),
        server_port(local_port)
{}

StateManager::~StateManager()
{}

void StateManager::setState(ServerState *newState)
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

ServerState* StateManager::getState()
{
    return current_state;
}

void StateManager::handleInput()
{
    current_state->handleInput();
}

void StateManager::init()
{
    log.print(LOG_PARANOID, "[ ] StateManager::init()\n");

    setState(new StateInit(this));
}

void StateManager::startServer()
{
    log.print(LOG_DEBUG, "[ ] Creating Cimabue Server...\n");

    server = new CimabueServer(server_port, true);

    if (server != NULL)
        log.print(LOG_DEBUG, "[ ] Created Cimabue Server (%d)\n", server_port);
    else
        log.print(LOG_ERROR, "[!] Error creating Cimabue Server (%d)\n", server_port);
}
