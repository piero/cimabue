/*
* ClientState.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef CLIENTSTATE_H_
#define CLIENTSTATE_H_

#include <string>
#include <log/Log.h>

class StateManager;


typedef enum
{
	STATE_NONE,
	STATE_INIT,
	STATE_CONNECTING,
	STATE_READY,
	STATE_QUIT
} state_t;


class ClientState
{
public:
	ClientState(StateManager *caller, state_t id = STATE_NONE);
    virtual ~ClientState();

    state_t getID();
    std::string getName();

    virtual void handleInput() = 0;
    virtual void onEntry() = 0;
    virtual void onExit() = 0;

protected:
    StateManager *manager;
    state_t stateID;

    Log log;
};


#endif /* CLIENTSTATE_H_ */
