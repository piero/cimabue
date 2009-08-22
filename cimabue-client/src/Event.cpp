/*
* Event.cpp
*
*  Created on: Aug 17, 2009
*      Author: piero
*/

#include "Event.h"

using namespace std;


Event::Event(event_type_t event_type)
{
    event.type = event_type;
}

Event::~Event()
{
    log.print(LOG_PARANOID, "[x] Deleting Event 0x%x\n", this);
}

event_type_t Event::getType()
{
    return event.type;
}

EventMessage::EventMessage(string sender_nick, string msg_content)
        : Event(EVT_NEW_MESSAGE)
{
    log.print(LOG_PARANOID, "[+] New EventMessage: 0x%x\n", this);

    memset(event.data.msg.nickname, 0, 128);
    memset(event.data.msg.message, 0, 1024);

    strncpy(event.data.msg.nickname, sender_nick.c_str(), sender_nick.length());
    strncpy(event.data.msg.message, msg_content.c_str(), msg_content.length());
}

char* EventMessage::getNickname()
{
    return event.data.msg.nickname;
}

char* EventMessage::getMessage()
{
    return event.data.msg.message;
}


EventConnecting::EventConnecting(string target_ip, unsigned target_port)
        : Event(EVT_CONNECTING)
{
    log.print(LOG_PARANOID, "[+] New EventConnecting: 0x%x\n", this);

    memset(event.data.connect.server_ip, 0, 16);

    strncpy(event.data.connect.server_ip, target_ip.c_str(), target_ip.length());
    event.data.connect.server_port = target_port;
}

char* EventConnecting::getTargetIP()
{
    return event.data.connect.server_ip;
}

unsigned EventConnecting::getTargetPort()
{
    return event.data.connect.server_port;
}


EventConnected::EventConnected(string target_ip, unsigned target_port)
        : Event(EVT_CONNECTED)
{
    log.print(LOG_PARANOID, "[+] New EventConnected: 0x%x\n", this);

    memset(event.data.connect.server_ip, 0, 16);

    strncpy(event.data.connect.server_ip, target_ip.c_str(), target_ip.length());
    event.data.connect.server_port = target_port;
}

char* EventConnected::getTargetIP()
{
    return event.data.connect.server_ip;
}

unsigned EventConnected::getTargetPort()
{
    return event.data.connect.server_port;
}


EventUpdateAdd::EventUpdateAdd(string nickname)
        : Event(EVT_UPDATE_CLIENT_LIST)
{
    log.print(LOG_PARANOID, "[+] New EventUpdateAdd: 0x%x\n", this);

    memset(event.data.msg.nickname, 0, 128);
    strncpy(event.data.msg.nickname, nickname.c_str(), nickname.length());
}

char* EventUpdateAdd::getNickname()
{
    return event.data.msg.nickname;
}
