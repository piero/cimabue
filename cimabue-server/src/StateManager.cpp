/*
* StateManager.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "StateManager.h"

using namespace std;

StateManager::StateManager(unsigned short local_port)
{}

StateManager::~StateManager()
{}

void StateManager::setState(ServerState *newState)
{
    current_state = newState;
}

ServerState* StateManager::getState()
{
    return current_state;
}

void StateManager::handleInput()
{
	current_state->handleInput();
}
