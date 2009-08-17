/*
* ConsoleView.h
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#ifndef CONSOLEVIEW_H_
#define CONSOLEVIEW_H_

#include "Observer.h"
#include <iostream>

class ConsoleView : public Observer
{
public:
    ConsoleView(StateManager *caller);
    virtual ~ConsoleView();

    void update(Event *event);

private:
    bool kill_console;
    pthread_t console_thread;
    bool console_thread_is_running;
    static void* do_console_thread(void *arg);
};


#endif /* CONSOLEVIEW_H_ */
