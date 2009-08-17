/*
* ConsoleView.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "ConsoleView.h"

using namespace std;


ConsoleView::ConsoleView(StateManager *caller)
        : Observer(caller)
{
    console_thread_is_running = false;
    kill_console = false;

    pthread_create(&console_thread, NULL, do_console_thread, this);
}

ConsoleView::~ConsoleView()
{
    kill_console = true;

    if (console_thread_is_running)
        pthread_join(console_thread, NULL);
}


void ConsoleView::update(event_t event)
{
    switch (event.type)
    {
    case EVT_CONNECTING:
        printf("[V] Connecting to %s:%d...\n", model->getServerIP().c_str(), model->getServerPort());
        break;

    case EVT_CONNECTED:
        printf("[V] Connected to %s:%d\n", model->getServerIP().c_str(), model->getServerPort());
        break;

    case EVT_NEW_MESSAGE:
    	printf("[V] Received new message\n\t%s\n", event.data.c_str());
        break;

    case EVT_UPDATE_CLIENT_LIST:
        break;

    case EVT_UPDATE_SERVER:
        break;

    case EVT_ERROR:
        break;

    default:
        break;
    }

}

void* ConsoleView::do_console_thread(void *arg)
{
    ConsoleView *me = (ConsoleView *)arg;

    me->console_thread_is_running = true;

    // Ask nickname
    string nickname = "", local_port = "", server_ip = "", server_port = "";

    cout << ">>> Nickname?" << endl;
    cin >> nickname;

    cout << ">>> Local port?" << endl;
    cin >> local_port;

    cout << ">>> Server IP?" << endl;
    cin >> server_ip;

    cout << ">>> Server port?" << endl;
    cin >> server_port;

    // Apply defaults
    if (nickname == "\\")
        nickname = "user";

    if (local_port == "\\")
        local_port = "6000";

    if (server_ip == "\\")
        server_ip = "172.16.17.131";

    if (server_port == "\\")
        server_port = "8000";

    // GO!!!
    me->model->init(nickname, atoi(local_port.c_str()), server_ip, atoi(server_port.c_str()));

    string destination, message;

    while (!me->kill_console)
    {
        cout << ">>> Destination?" << endl;
        cin >> destination;

        cout << ">>> Message?" << endl;
        cin >> message;

        // TODO: Send message
        me->model->getConnector()->sendMessage(destination, message);
    }

    me->console_thread_is_running = false;
    pthread_exit(0);
}
