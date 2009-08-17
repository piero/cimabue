/*
* StateInit.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef STATEINIT_H_
#define STATEINIT_H_

#include "ClientState.h"


class StateInit : public ClientState
{
public:
    StateInit(StateManager *caller);
    virtual ~StateInit();

    void handleInput();

protected:
    void onEntry();
    void onExit();
};


#endif /* STATEINIT_H_ */
