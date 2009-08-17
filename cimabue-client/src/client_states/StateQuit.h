/*
* StateQuit.h
*
*  Created on: Aug 17, 2009
*      Author: piero
*/

#ifndef STATEQUIT_H_
#define STATEQUIT_H_

#include "ClientState.h"

class StateQuit : public ClientState
{
public:
    StateQuit(StateManager *caller);
    virtual ~StateQuit();

    void handleInput();

protected:
    void onEntry();
    void onExit();
};


#endif /* STATEQUIT_H_ */
