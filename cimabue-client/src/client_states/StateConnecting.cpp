/*
* StateConnecting.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "StateConnecting.h"
#include "StateReady.h"
#include "../StateManager.h"

StateConnecting::StateConnecting(StateManager *caller)
        : ClientState(caller, STATE_CONNECTING)
{}

StateConnecting::~StateConnecting()
{}

void StateConnecting::onEntry()
{
	manager->connectToServer();

	// Go to Ready state
	manager->setState(new StateReady(manager));
	delete this;
}

void StateConnecting::onExit()
{
}

void StateConnecting::handleInput()
{

}
