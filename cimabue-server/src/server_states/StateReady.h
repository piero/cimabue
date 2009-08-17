/*
* StateReady.h
*
*  Created on: Aug 17, 2009
*      Author: piero
*/

#ifndef STATEREADY_H_
#define STATEREADY_H_

#include "ServerState.h"

class StateReady : public ServerState
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
