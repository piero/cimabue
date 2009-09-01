/*
* Node.h
*
*  Created on: 1 Sep 2009
*      Author: piero
*/

#ifndef NODE_H_
#define NODE_H_

#include "../base/base_include.h"


#if !defined(__APPLE__)
typedef long timestamp_t;
#else
typedef uint64_t timestamp_t;
#endif


class Node
{
public:
	Node();
	virtual ~Node();

	std::string getName();

protected:
    std::string name;
    Log log;

    timestamp_t getTimestamp();
};


#endif /* NODE_H_ */
