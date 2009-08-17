/*
 * main.cpp
 *
 *  Created on: Aug 17, 2009
 *      Author: piero
 */

#include <cimabue.h>
#include <iostream>
#include <signal.h>
#include "StateManager.h"

using namespace std;

// SIGINT handler
bool quit = false;

void sigint_handler(int sig)
{
	quit = true;
}


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Syntax: %s local_port\n", argv[0]);
		exit(0);
	}

	signal(SIGINT, &sigint_handler);

	Log::setLogLevel(LOG_DEBUG);

	printf("Creating State Manager...\n");

	StateManager manager(atoi(argv[1]));
	manager.init();

	while (!quit)
	{
		sleep(1);
	}

	printf("\nBye :-)\n");
}
