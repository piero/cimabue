/*
 * main.cpp
 *
 *  Created on: Aug 17, 2009
 *      Author: piero
 */

#include <cimabue.h>
#include <iostream>
#include "StateManager.h"

using namespace std;


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Syntax: %s local_port\n", argv[0]);
		exit(0);
	}

	Log::setLogLevel(LOG_DEBUG);

	StateManager manager(atoi(argv[1]));
}
