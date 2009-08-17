/*
* ConsoleView.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "ConsoleView.h"

ConsoleView::ConsoleView(StateManager *caller)
        : Observer(caller)
{}

ConsoleView::~ConsoleView()
{}


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
