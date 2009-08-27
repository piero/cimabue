/*
* Filter.h
*
*  Created on: 26 Aug 2009
*      Author: piero
*/

#ifndef FILTER_H_
#define FILTER_H_

#include "Event.h"


class Filter
{
public:
    Filter(std::string event_type)
    {
        eventType = event_type;
    }

    virtual ~Filter()
    {}

    std::string getEventType()
    {
        return eventType;
    }

    virtual bool apply(Event *event) = 0;

protected:
    std::string eventType;
};


#endif /* FILTER_H_ */
