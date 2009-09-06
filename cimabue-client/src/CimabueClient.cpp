/*
* CimabueClient.cpp
*
*  Created on: May 4, 2009
*      Author: piero
*/

#include "CimabueClient.h"
#include "StateManager.h"

using namespace std;

CimabueClient::CimabueClient(StateManager *caller, unsigned short clientPort)
{
    manager = caller;

    nickname = manager->getNickname();

    server_proxy = new ServerProxy(this, clientPort);
}

CimabueClient::~CimabueClient()
{
    delete server_proxy;
}

void CimabueClient::connectToServer(std::string serverIP, unsigned short serverPort)
{
    // Notify views
    EventConnecting event_connecting(serverIP, serverPort);
    manager->updateViews(event_connecting);

    while (!server_proxy->isConnected())
    {
        string error;

        if (server_proxy->connect(serverIP, serverPort, error) != RET_SUCCESS)
        {
            log.print(LOG_ERROR, "[!] Error connecting to Server (%s:%d): %s\n",
                      serverIP.c_str(), serverPort, error.c_str());

            // Notify views
            string error_msg = "Error connecting to Server (";
            error_msg += serverIP;
            error_msg += ":";
            error_msg += serverPort;
            error_msg += "): ";
            error_msg += error;
            EventError event_error(error_msg);
            manager->updateViews(event_error);

            // Retry
            sleep(2);
        }
    }

    log.print(LOG_INFO, "[ ] Connected to Server (%s:%d)\n",
              serverIP.c_str(), serverPort);

    // Notify views
    EventConnected event_connected(serverIP, serverPort);
    manager->updateViews(event_connected);
}

int CimabueClient::sendMessage(string dest, string content)
{
    int ret = RET_ERROR;

    map<string, string>::iterator iter = clientNickToNameMap.find(dest);

    if (iter == clientNickToNameMap.end())
    {
        log.print(LOG_ERROR, "[!] Couldn't find \"%s\" in client list\n", dest.c_str());

        // Notify views
        string error_msg = "Couldn't find \"";
        error_msg += dest;
        error_msg += "\"";
        EventError event_error(error_msg);
        manager->updateViews(event_error);

        return ret;
    }

    dest = iter->second;

    string data_to_send = nickname;
    data_to_send += DATA_SEPARATOR;
    data_to_send += content;

    ret = server_proxy->sendMessage(dest, data_to_send);

    if (ret != RET_SUCCESS)
    {
        log.print(LOG_ERROR, "[!] Couldn't send message to \"%s\"\n", dest.c_str());

        // Notify views
        string error_msg = "Couldn't send message to \"";
        error_msg += dest;
        error_msg += "\"";
        EventError event_error(error_msg);
        manager->updateViews(event_error);
    }
    else
    {
    	// Notify views: simulate a received message from ourselves
    	EventMessage msg_evt(nickname, content);
    	manager->updateViews(msg_evt);
    }

    return ret;
}

string CimabueClient::getNickname()
{
    return nickname;
}

void CimabueClient::extractNicknameAndData(string s, string &nick, string &data)
{
    string::size_type lastPos = s.find_first_not_of(DATA_SEPARATOR, 0);
    string::size_type pos = s.find_first_of(DATA_SEPARATOR, 0);

    list<string> tokens;

    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(s.substr(lastPos, pos - lastPos));
        lastPos = s.find_first_not_of(DATA_SEPARATOR, pos);
        pos = s.find_first_of(DATA_SEPARATOR, lastPos);
    }

    nick = tokens.front();
    tokens.pop_front();
    data = tokens.front();
}

pair<string, string> CimabueClient::extractNewClient(std::string &data)
{
    string nick, name;
    size_t first_separator = data.find(":");

    nick = data.substr(0, first_separator);

    size_t second_separator = data.find(":", first_separator + 1);
    if (second_separator != string::npos)
    {
        name = data.substr(first_separator + 1, second_separator - first_separator - 1);
        data = data.substr(second_separator + 1);
    }
    else
    {
        // Last entry
        name = data.substr(first_separator + 1);
        data = MSG_VOID;
    }

    return pair<string, string>(nick, name);
}
