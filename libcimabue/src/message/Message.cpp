/*
* Message.cpp
*
*  Created on: Apr 3, 2009
*      Author: piero
*/

#include "Message.h"

using namespace std;

Message::Message()
        : type(MSG_NONE),
        clientSrc(MSG_VOID),
        clientDest(MSG_VOID),
        serverSrc(MSG_VOID),
        serverDest(MSG_VOID),
        data(MSG_VOID),
        priority(MSG_PRIORITY_NORMAL)
{
    messageLog.print(LOG_PARANOID, "[o] Message: 0x%x\n", this);
}

Message::Message(string msg)
{
    Message();
    deserialize(msg);
}

Message::Message(message_t msg_type,
                 string fromClient, string toClient,
                 string fromServer, string toServer,
                 string content, message_priority_t msg_priority)
        : type(msg_type),
        clientSrc(fromClient),
        clientDest(toClient),
        serverSrc(fromServer),
        serverDest(toServer),
        data(content),
        priority(msg_priority)
{
    //separator = MSG_SEPARATOR;
    messageLog.print(LOG_PARANOID, "[o] Message: 0x%x\n", this);
}

Message::~Message()
{
    messageLog.print(LOG_PARANOID, "[x] Message: 0x%x\n", this);
}

int Message::Recv(int skt)
{
    string received;
    char *recv_buf = (char*)malloc(sizeof(char) * MSG_BUFFER_SIZE);
    memset(recv_buf, 0, MSG_BUFFER_SIZE);
    int recv_bytes = 0;
    bool termination = false;

    while (recv_bytes < MSG_BUFFER_SIZE && !termination)
    {
        int rd = recv(skt, (void*)(recv_buf + recv_bytes), (size_t)1, 0);

        if (rd < 0)
        {
            messageLog.print(LOG_ERROR, "[!] recv() failed: %s\n", strerror(errno));
            return RET_ERROR;
        }
        else if (rd == 0)
        {
            messageLog.print(LOG_ERROR, "[!] Unexpected end of message\n");
            return RET_ERROR;
        }

        if (*(recv_buf + recv_bytes) == '|')
        {
            termination = true;
            messageLog.print(LOG_PARANOID, "End of message found\n");
        }

        recv_bytes++;
    }

    messageLog.print(LOG_PARANOID, "[ ] Received %d bytes\n%s\n\n", recv_bytes, recv_buf);

    recv_buf[recv_bytes - 1] = '\0';	// Remove termination char
    received = string(recv_buf);
    free(recv_buf);

    deserialize(received);

    messageLog.print(LOG_PARANOID, "--- RECEIVED MESSAGE ---\n");
    dump();
    messageLog.print(LOG_PARANOID, "------------------------\n");

    return RET_SUCCESS;
}


int Message::Reply(int skt)
{
    messageLog.print(LOG_PARANOID, "[ ] Replying on socket %d...\n", skt);

    string message = serialize();

    if (send(skt, message.c_str(), message.size(), 0) != (int)message.size())
    {
        messageLog.print(LOG_ERROR, "[!] Error  replying on socket %d : %s\n",
                         skt, strerror(errno));

        return RET_ERROR;
    }

    messageLog.print(LOG_PARANOID, "-------- SENT REPLY MESSAGE --------\n");
    dump();
    messageLog.print(LOG_PARANOID, "------------------------------------\n");

    return RET_SUCCESS;
}


Message* Message::Send(std::string ip, unsigned int port)
{
    messageLog.print(LOG_DEBUG, "Message::Send(%s, %d)\n", ip.c_str(), port);

    int skt = socket(PF_INET, SOCK_STREAM, 0);

    if (skt < 0)
    {
        messageLog.print(LOG_ERROR, "[!] Error creating socket to %s:%d - %s\n",
                         ip.c_str(), port, strerror(errno));

        return new Message(MSG_ERROR,
                           MSG_VOID, MSG_VOID,
                           MSG_VOID, MSG_VOID,
                           "Error creating socket");
    }

    // Set socket timeouts
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(skt, SOL_SOCKET, SO_SNDTIMEO, (void*)&timeout, sizeof(timeout));
    setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, (void*)&timeout, sizeof(timeout));

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(port);

    if (connect(skt, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        messageLog.print(LOG_ERROR, "[!] Error connecting to %s:%d - %s\n",
                         ip.c_str(), port, strerror(errno));

        close(skt);
        return new Message(MSG_ERROR,
                           MSG_VOID, MSG_VOID,
                           MSG_VOID, MSG_VOID,
                           "Error connecting");
    }

    messageLog.print(LOG_DEBUG, "[ ] Sending message to %s:%d...\n", ip.c_str(), port);

    string message = serialize();
    int sent_bytes = send(skt, message.c_str(), message.size(), 0);
    if (sent_bytes != (int)message.size())
    {
        messageLog.print(LOG_ERROR, "[!] Error sending message to %s:%d - %s\n",
                         ip.c_str(), port, strerror(errno));

        close(skt);
        return new Message(MSG_ERROR,
                           MSG_VOID, MSG_VOID,
                           MSG_VOID, MSG_VOID,
                           "Error sending message");
    }

    messageLog.print(LOG_PARANOID, "--- SENT MESSAGE (%s:%d) ---\n", ip.c_str(), port);
    dump();
    messageLog.print(LOG_PARANOID, "---------------------------- %d bytes\n", sent_bytes);


    // Wait for a reply
    Message *reply = new Message(MSG_NONE, "", "", "", "", "");
    int ret = reply->Recv(skt);
    close (skt);

    if (ret != RET_SUCCESS)
    {
        messageLog.print(LOG_ERROR, "[!] Error receiving reply\n");

        close(skt);
        delete reply;
        return new Message(MSG_ERROR,
                           MSG_VOID, MSG_VOID,
                           MSG_VOID, MSG_VOID,
                           "Error receiving reply");
    }

    close(skt);
    return reply;
}


void Message::copyTo(Message &to)
{
    to.setType(type);
    to.setClientSource(clientSrc);
    to.setClientDest(clientDest);
    to.setServerSource(serverSrc);
    to.setServerDest(serverDest);
    to.setData(data);
    to.setPriority(priority);
}

// Client Source
string Message::getClientSource()
{
    return clientSrc;
}

void Message::setClientSource(string s)
{
    clientSrc = s;
}

// Client Dest
string Message::getClientDest()
{
    return clientDest;
}

void Message::setClientDest(string d)
{
    clientDest = d;
}

// Server Source
string Message::getServerSource()
{
    return serverSrc;
}

void Message::setServerSource(string s)
{
    serverSrc = s;
}

// Server Dest
string Message::getServerDest()
{
    return serverDest;
}

void Message::setServerDest(string d)
{
    serverDest = d;
}

message_t Message::getType()
{
    return type;
}

void Message::setType(message_t t)
{
    type = t;
}

message_priority_t Message::getPriority()
{
    return priority;
}

void Message::setPriority(message_priority_t p)
{
    priority = p;
}

void Message::setData(string d)
{
    data = d;
}

string Message::getData()
{
    return data;
}

string Message::serialize()
{
    string serialized;

    serialized = decodeType(type);
    serialized += MSG_SEPARATOR + clientSrc;
    serialized += MSG_SEPARATOR + clientDest;
    serialized += MSG_SEPARATOR + serverSrc;
    serialized += MSG_SEPARATOR + serverDest;
    serialized += MSG_SEPARATOR + data;
    serialized += MSG_SEPARATOR + decodePriority(priority);
    serialized += MSG_END;

    messageLog.print(LOG_PARANOID, "Serialized message: \"%s\" (%d bytes)\n",
                     serialized.c_str(), serialized.size());

    return serialized;
}

void Message::deserialize(string s)
{
    messageLog.print(LOG_PARANOID, "Deserializing: \"%s\"...\n", s.c_str());

    string::size_type lastPos = s.find_first_not_of(MSG_SEPARATOR, 0);
    string::size_type pos = s.find_first_of(MSG_SEPARATOR, 0);

    list<string> tokens;

    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(MSG_SEPARATOR, pos);
        pos = s.find_first_of(MSG_SEPARATOR, lastPos);
    }

    type = encodeType(tokens.front()); // type
    tokens.pop_front();
    clientSrc = tokens.front(); // clientSrc
    tokens.pop_front();
    clientDest = tokens.front(); // clientDest
    tokens.pop_front();
    serverSrc = tokens.front(); // serverSrc
    tokens.pop_front();
    serverDest = tokens.front(); // serverDest
    tokens.pop_front();
    data = tokens.front(); // data
    tokens.pop_front();
    priority = encodePriority(tokens.front()); // priority
    tokens.pop_front();
}

message_t Message::encodeType(std::string s)
{
    if (s == "MSG_ADD_CLIENT")
        return MSG_ADD_CLIENT;

    else if (s == "MSG_UPDATE_CLIENTS")
        return MSG_UPDATE_CLIENTS;

    else if (s == "MSG_REM_CLIENT")
        return MSG_REM_CLIENT;

    else if (s == "MSG_USER")
        return MSG_USER;

    else if (s == "MSG_SEND_MESSAGE")
        return MSG_SEND_MESSAGE;

    else if (s == "MSG_PING_CLIENT")
        return MSG_PING_CLIENT;

    else if (s == "MSG_ERROR")
        return MSG_ERROR;

    else
        return MSG_NONE;
}

string Message::decodeType(message_t t)
{
    string ret = "MSG_NONE";

    switch (t)
    {
    case MSG_ADD_CLIENT:
        ret = "MSG_ADD_CLIENT";
        break;

    case MSG_UPDATE_CLIENTS:
        ret = "MSG_UPDATE_CLIENTS";
        break;

    case MSG_REM_CLIENT:
        ret = "MSG_REM_CLIENT";
        break;

    case MSG_SEND_MESSAGE:
        ret = "MSG_SEND_MESSAGE";
        break;

    case MSG_PING_CLIENT:
        ret = "MSG_PING_CLIENT";
        break;

    case MSG_USER:
        ret = "MSG_USER";
        break;

    case MSG_ERROR:
        ret = "MSG_ERROR";
        break;

    default:
        ret = "MSG_NONE";
        break;
    }

    return ret;
}


string Message::typeToString(message_t type)
{
    return decodeType(type);
}


message_priority_t Message::encodePriority(std::string s)
{
    // TODO
    return MSG_PRIORITY_NORMAL;
}

string Message::decodePriority(message_priority_t p)
{
    // TODO
    return "MSG_PRIORITY_NORMAL";
}

void Message::dump()
{
    messageLog.print(LOG_PARANOID, "Type:\t%s\n", decodeType(type).c_str());
    messageLog.print(LOG_PARANOID, "clientSrc:\t%s\n", clientSrc.c_str());
    messageLog.print(LOG_PARANOID, "clientDest:\t%s\n", clientDest.c_str());
    messageLog.print(LOG_PARANOID, "serverSrc:\t%s\n", serverSrc.c_str());
    messageLog.print(LOG_PARANOID, "serverDest:\t%s\n", serverDest.c_str());
    messageLog.print(LOG_PARANOID, "Data:\t%s\n", data.c_str());
    messageLog.print(LOG_PARANOID, "Prio:\t%d\n", priority);
}


bool Message::isErrorMessage()
{
    if (type == MSG_ERROR)
        return true;
    else
        return false;
}
