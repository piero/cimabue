/*
* Event.h
*
*  Created on: 26 Aug 2009
*      Author: piero
*/

#ifndef EVENT_H_
#define EVENT_H_

#include <string>


class Event
{
public:
    Event(std::string type)
    {
        eventType = type;
    }

    virtual ~Event()
    {}

    std::string getEventType()
    {
        return eventType;
    }

protected:
    std::string eventType;
};


#endif /* EVENT_H_ */
