/*
* Node.cpp
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#include "Node.h"

using namespace std;


Node::Node()
{
	// Generate node name
	char *temp_name;
	temp_name = (char*) malloc(sizeof(char) * 128);

#if !defined(__APPLE__)

	sprintf(temp_name, "%ld", getTimestamp());
#else

	sprintf(temp_name, "%lld", getTimestamp());
#endif

	name = string((const char*) temp_name);
	free(temp_name);
}

Node::~Node()
{
	// TODO Auto-generated destructor stub
}

string Node::getName()
{
	return name;
}

timestamp_t Node::getTimestamp()
{
#if !defined(__APPLE__)
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (long) (ts.tv_sec * 1E9 + ts.tv_nsec);
#else

	return mach_absolute_time();
#endif
}
