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

    // Accessors
    std::string getType()
    {
        return eventType;
    }

    void setType(std::string type)
    {
    	eventType = type;
    }

    std::string getData()
    {
    	return eventData;
    }

    void setData(std::string data)
    {
    	eventData = data;
    }

protected:
    std::string eventType;
    std::string eventData;
};


#endif /* EVENT_H_ */
