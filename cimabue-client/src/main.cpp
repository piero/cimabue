/*
* main.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/


#include <cimabue.h>
#include <iostream>
#include "StateManager.h"
#include "view/ConsoleView.h"

using namespace std;

// SIGINT handler
bool quit = false;

void sigint_handler(int sig)
{
    quit = true;
}


int main(int argc, char *argv[])
{
    Log::setLogLevel(LOG_DEBUG);

    StateManager manager;
    ConsoleView console(&manager);

    while (!quit)
    {
        sleep(1);
    }

    printf("\nBye :-)\n");
}
