/*
* ServerState.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "ServerState.h"

using namespace std;


ServerState::ServerState(StateManager *caller, state_t id)
        : manager(caller),
        stateID(id),
        last_event(EVT_NONE)
{
    onEntry();
}

ServerState::~ServerState()
{
    onExit();
}

state_t ServerState::getID()
{
    return stateID;
}


event_t ServerState::getLastEvent()
{
	return last_event;
}


string ServerState::getName()
{
	string state_name;

	switch(stateID)
	{
	case STATE_INIT:
		state_name = "INIT";
		break;

	case STATE_READY:
		state_name = "READY";
		break;

	case STATE_QUIT:
		state_name = "QUIT";
		break;

	default:
		state_name = "INVALID STATE";
		break;
	}

	return state_name;
}
