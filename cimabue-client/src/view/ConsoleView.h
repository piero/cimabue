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

    void update();

private:
    bool kill_console;
    pthread_t input_thread;
    bool input_thread_is_running;
    static void* do_input_thread(void *arg);
};


#endif /* CONSOLEVIEW_H_ */
