/*
* Observer.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef OBSERVER_H_
#define OBSERVER_H_

#include "../StateManager.h"
#include <log/Log.h>

class Observer
{
public:
    Observer(StateManager *caller);
    virtual ~Observer();

    virtual void update(Event *event) = 0;

protected:
    StateManager *model;
    Log log;
};


#endif /* OBSERVER_H_ */
