/*
* ConsoleView.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef CONSOLEVIEW_H_
#define CONSOLEVIEW_H_

#include "Observer.h"

class ConsoleView : public Observer
{
public:
    ConsoleView(StateManager *caller);
    virtual ~ConsoleView();

    void update();
};


#endif /* CONSOLEVIEW_H_ */
