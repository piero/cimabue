/*
 * Event.h
 *
 *  Created on: Aug 17, 2009
 *      Author: piero
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <string.h>
#include <string>
#include <log/Log.h>


typedef enum
{
    EVT_NONE,
    EVT_ERROR,
    EVT_CONNECTING,
    EVT_CONNECTED,
    EVT_NEW_MESSAGE,
    EVT_UPDATE_CLIENT_LIST_ADD,
    EVT_UPDATE_CLIENT_LIST_REM,
    EVT_UPDATE_SERVER
} event_type_t;

typedef struct
{
    char error_string[256];
}
event_error_data_t;

typedef struct
{
    char nickname[128];
    char message[1024];
}
event_message_data_t;

typedef struct
{
    char server_ip[16];
    unsigned short server_port;
}
event_connection_data_t;

typedef struct
{
	char nickname[128];
} event_update_clients_t;

typedef struct
{
    event_type_t type;

    union
    {
    	event_error_data_t error;
        event_message_data_t msg;
        event_connection_data_t connect;
        event_update_clients_t update;
    } data;

}
event_t;


class Event
{
public:
	Event(event_type_t type);
	virtual ~Event();

	event_type_t getType();

protected:
	event_t event;
	event_type_t type;

	Log log;
};


class EventError : public Event
{
public:
	EventError(std::string error);

	char* getError();
};

class EventMessage : public Event
{
public:
	EventMessage(std::string sender_nick, std::string msg_content);

	char* getNickname();
	char* getMessage();
};

class EventConnecting : public Event
{
public:
	EventConnecting(std::string target_ip, unsigned short target_port);

	char* getTargetIP();
	unsigned short getTargetPort();
};

class EventConnected : public Event
{
public:
	EventConnected(std::string target_ip, unsigned short target_port);

	char* getTargetIP();
	unsigned short getTargetPort();
};

class EventUpdateAdd : public Event
{
public:
	EventUpdateAdd(std::string nickname);

	char* getNickname();
};

class EventUpdateRem : public Event
{
public:
	EventUpdateRem(std::string nickname);

	char* getNickname();
};

#endif /* EVENT_H_ */
