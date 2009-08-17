/*
* ConsoleView.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "ConsoleView.h"

using namespace std;


ConsoleView::ConsoleView(StateManager *caller)
        : Observer(caller)
{
	input_thread_is_running = false;
	kill_console = false;

	pthread_create(&input_thread, NULL, do_input_thread, this);
}

ConsoleView::~ConsoleView()
{
	kill_console = true;

	if (input_thread_is_running)
		pthread_join(input_thread, NULL);
}


void ConsoleView::update()
{
    switch (model->getState()->getLastEvent())
    {
    case EVT_CONNECTING:
    	printf("[V] Connecting to %s:%d...\n", model->getServerIP().c_str(), model->getServerPort());
    	break;

    case EVT_CONNECTED:
    	printf("[V] Connected to %s:%d\n", model->getServerIP().c_str(), model->getServerPort());
    	break;

    case EVT_NEW_MESSAGE:
        break;

    case EVT_UPDATE_CLIENT_LIST:
        break;

    case EVT_UPDATE_SERVER:
    	break;

    case EVT_ERROR:
    	break;

    default:
    	break;
    }

}

void* ConsoleView::do_input_thread(void *arg)
{
	ConsoleView *me = (ConsoleView *)arg;

	me->input_thread_is_running = true;

	string destination, message;

	while (!me->kill_console)
	{
		cout << ">>> Destination?" << endl;
		cin >> destination;

		cout << ">>> Message?" << endl;
		cin >> message;

		// TODO: Send message
	}

	me->input_thread_is_running = false;
	pthread_exit(0);
}
