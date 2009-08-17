/*
* StateInit.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef STATEINIT_H_
#define STATEINIT_H_

#include "ServerState.h"


class StateInit : public ServerState
{
public:
    StateInit(StateManager *caller);
    virtual ~StateInit();

    void handleInput();
    void onEntry();
    void onExit();

protected:
};


#endif /* STATEINIT_H_ */
