/*
* StateReady.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef STATEREADY_H_
#define STATEREADY_H_

#include "ClientState.h"

class StateReady : public ClientState
{
public:
    StateReady(StateManager *caller);
    virtual ~StateReady();

    void handleInput();

protected:
    void onEntry();
    void onExit();
};


#endif /* STATEREADY_H_ */
