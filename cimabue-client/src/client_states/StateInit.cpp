/*
* StateInit.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "StateInit.h"
#include "../StateManager.h"
#include "StateConnecting.h"

StateInit::StateInit(StateManager *caller)
        : ClientState(caller, STATE_INIT)
{}

StateInit::~StateInit()
{}

void StateInit::onEntry()
{
    manager->setState(new StateConnecting(manager));
    delete this;
}

void StateInit::onExit()
{
}

void StateInit::handleInput()
{
}
