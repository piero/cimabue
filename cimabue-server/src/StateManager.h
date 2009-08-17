/*
* StateManager.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef STATEMANAGER_H_
#define STATEMANAGER_H_

#include "server_states/StateInit.h"
#include "nodes/CimabueServer.h"
#include <list>

class Observer;

class StateManager
{
public:
    StateManager(unsigned short local_port);
    virtual ~StateManager();

    void setState(ServerState *newState);
    ServerState* getState();

    void handleInput();

private:
    ServerState *current_state;
    Log log;
};


#endif /* STATEMANAGER_H_ */
