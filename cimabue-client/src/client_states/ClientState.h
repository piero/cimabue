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


typedef enum
{
	EVT_NONE,
	EVT_ERROR,
	EVT_CONNECTING,
	EVT_CONNECTED,
	EVT_NEW_MESSAGE,
	EVT_UPDATE_CLIENT_LIST,
	EVT_UPDATE_SERVER
} event_t;


class ClientState
{
public:
	ClientState(StateManager *caller, state_t id = STATE_NONE);
    virtual ~ClientState();

    state_t getID();
    event_t getLastEvent();
    std::string getName();

    virtual void handleInput() = 0;

protected:
    virtual void onEntry() {};
    virtual void onExit() {};

    StateManager *manager;
    state_t stateID;
    event_t last_event;

    Log log;
};


#endif /* CLIENTSTATE_H_ */
