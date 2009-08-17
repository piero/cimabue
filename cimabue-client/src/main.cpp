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
    if (argc < 4)
    {
        fprintf(stderr, "Syntax: %s local_port server_ip server_port\n", argv[0]);
        exit(0);
    }

    Log::setLogLevel(LOG_DEBUG);

    StateManager manager(atoi(argv[1]), argv[2], atoi(argv[3]));
    ConsoleView console(&manager);

    manager.init();

    while (!quit)
    {
        sleep(1);
    }

    printf("\nBye :-)\n");
}
