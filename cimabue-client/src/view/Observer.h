/*
* Observer.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef OBSERVER_H_
#define OBSERVER_H_

#include "../StateManager.h"

class Observer
{
public:
    Observer(StateManager *caller);
    virtual ~Observer();

    virtual void update() = 0;

protected:
    StateManager *model;
};


#endif /* OBSERVER_H_ */
