/*
* ServerState.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef SERVERSTATE_H_
#define SERVERSTATE_H_

#include <string>
#include <log/Log.h>

class StateManager;


typedef enum
{
	STATE_NONE,
	STATE_INIT,
	STATE_READY,
	STATE_QUIT
} state_t;


typedef enum
{
	EVT_NONE,
	EVT_ERROR,
	EVT_NEW_MESSAGE,
	EVT_UPDATE_CLIENT_LIST,
	EVT_UPDATE_SERVER
} event_t;


class ServerState
{
public:
	ServerState(StateManager *caller, state_t id = STATE_NONE);
    virtual ~ServerState();

    state_t getID();
    event_t getLastEvent();
    std::string getName();

    virtual void handleInput() = 0;
    virtual void onEntry() = 0;
    virtual void onExit() = 0;

protected:

    StateManager *manager;
    state_t stateID;
    event_t last_event;

    Log log;
};


#endif /* SERVERSTATE_H_ */
