/*
* StateConnecting.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "StateConnecting.h"
#include "../StateManager.h"

StateConnecting::StateConnecting(StateManager *caller)
        : ClientState(caller, STATE_CONNECTING)
{}

StateConnecting::~StateConnecting()
{}

void StateConnecting::onEntry()
{
	// Connect to Server
	last_event = EVT_CONNECTING;
	manager->updateViews();

	manager->connectToServer();
}

void StateConnecting::onExit()
{
}

void StateConnecting::handleInput()
{

}
