/*
* StateConnecting.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef STATECONNECTING_H_
#define STATECONNECTING_H_

#include "ClientState.h"

class StateConnecting : public ClientState
{
public:
    StateConnecting(StateManager *caller);
    virtual ~StateConnecting();

    void handleInput();
    void onEntry();
    void onExit();
};


#endif /* STATECONNECTING_H_ */
