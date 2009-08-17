/*
* ClientState.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "ClientState.h"

using namespace std;


ClientState::ClientState(StateManager *caller, state_t id)
        : manager(caller),
        stateID(id),
        last_event(EVT_NONE)
{
    onEntry();
}

ClientState::~ClientState()
{
    onExit();
}

state_t ClientState::getID()
{
    return stateID;
}


event_t ClientState::getLastEvent()
{
    return last_event;
}


string ClientState::getName()
{
    string state_name;

    switch (stateID)
    {
    case STATE_INIT:
        state_name = "INIT";
        break;

    case STATE_CONNECTING:
        state_name = "CONNECTING";
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