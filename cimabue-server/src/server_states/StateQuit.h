/*
* StateQuit.h
*
*  Created on: Aug 17, 2009
*      Author: piero
*/

#ifndef STATEQUIT_H_
#define STATEQUIT_H_

#include "ServerState.h"

class StateQuit : public ServerState
{
public:
    StateQuit(StateManager *caller);
    virtual ~StateQuit();

    void handleInput();
    void onEntry();
    void onExit();

protected:
};


#endif /* STATEQUIT_H_ */
