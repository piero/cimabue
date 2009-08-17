/*
* StateInit.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "StateInit.h"
#include "StateReady.h"
#include "../StateManager.h"

StateInit::StateInit(StateManager *caller)
: ServerState(caller, STATE_INIT)
{}

StateInit::~StateInit()
{}


void StateInit::onEntry()
{
	manager->startServer();

	manager->setState(new StateReady(manager));
	delete this;
}

void StateInit::onExit()
{}

void StateInit::handleInput()
{}
