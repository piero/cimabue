/*
* StateQuit.cpp
*
*  Created on: Aug 17, 2009
*      Author: piero
*/

#include "StateQuit.h"

StateQuit::StateQuit(StateManager *caller)
: ServerState(caller, STATE_QUIT)
{}

StateQuit::~StateQuit()
{}


void StateQuit::onEntry()
{

}

void StateQuit::onExit()
{

}

void StateQuit::handleInput()
{

}
