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


void ConsoleView::update(Event *event)
{
    log.print(LOG_DEBUG, "--> Updating ConsoleView\n");

    switch (event->getType())
    {
    case EVT_CONNECTING:
        printf("[V] Connecting to %s:%d...\n",
               ((EventConnecting*)event)->getTargetIP(),
               ((EventConnecting*)event)->getTargetPort());
        break;

    case EVT_CONNECTED:
        printf("[V] Connected to %s:%d\n",
               ((EventConnected*)event)->getTargetIP(),
               ((EventConnected*)event)->getTargetPort());
        break;

    case EVT_NEW_MESSAGE:
        printf("[V] Received new message\n\t%s: %s\n",
               ((EventMessage*)event)->getNickname(),
               ((EventMessage*)event)->getMessage());
        break;

    case EVT_UPDATE_CLIENT_LIST_ADD:
        printf("[V] A new client has joined\n\t%s\n",
               ((EventMessage*)event)->getNickname());

        clients.push_back(((EventMessage*)event)->getNickname());
        dumpClients();
        break;

    case EVT_UPDATE_CLIENT_LIST_REM:
        {
            printf("[V] A client has left\n\t%s\n",
                   ((EventMessage*)event)->getNickname());

            list<string>::iterator iter;
            for (iter = clients.begin(); iter != clients.end(); iter++)
            {
                if (*iter == ((EventMessage*)event)->getNickname())
                {
                    clients.erase(iter);
                    break;
                }
            }

            dumpClients();
        }
        break;

    case EVT_UPDATE_SERVER:
        break;

    case EVT_ERROR:
        printf("[V] ERROR!\n\t%s\n", ((EventError*)event)->getError());
        break;

    default:
        log.print(LOG_WARNING, "[!] Unhandled event type: %d\n", event->getType());
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

        me->model->getConnector()->sendMessage(destination, message);
    }

    me->console_thread_is_running = false;
    pthread_exit(0);
}

void ConsoleView::dumpClients()
{
    list<string>::iterator iter;

    printf("CLIENTS:\n");

    for (iter = clients.begin(); iter != clients.end(); ++iter)
        printf("[V] * %s\n", iter->c_str());

    printf("\n");
}
