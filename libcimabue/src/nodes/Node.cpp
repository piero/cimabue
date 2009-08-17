/*
* Node.cpp
*
*  Created on: May 4, 2009
*      Author: piero
*/

#include "Node.h"

using namespace std;

Node::Node(unsigned short upPort,
           unsigned short DownPort,
           long timeout)
        : listen_thread_is_running(false),
        process_thread_is_running(false),
        die(false),
        selectTimeout(timeout)
{
	getLocalIPAddress(ip, "eth0");
    portList[UP_PORT] = upPort;
    portList[DOWN_PORT] = DownPort;

#if defined(__APPLE__) && defined(__MACH__)
    // TODO: http://kevincathey.com/code/using-counting-semaphores-on-mac-os-x/
    // TODO: OR USE NAMED SEMAPHORES (easier, maybe)
#else

    sem_init(&ipc_sem, 0, 0);
#endif

    // Generate node name
    char *temp_name;
    temp_name = (char*) malloc(sizeof(char) * 128);

#if !defined(__APPLE__)

    sprintf(temp_name, "%ld", getTimestamp());
#else

    sprintf(temp_name, "%lld", getTimestamp());
#endif

    name = string((const char*) temp_name);
    free(temp_name);

    pthread_create(&process_thread, NULL, do_process_thread, this);
    pthread_create(&listen_thread, NULL, do_listen_thread, this);

    log.print(LOG_INFO, "Created node %s (%s:%d-%d)\n", name.c_str(),
              ip.c_str(), portList[UP_PORT], portList[DOWN_PORT]);
}

Node::~Node()
{
    die = true;

    pthread_join(listen_thread, NULL);
    pthread_join(process_thread, NULL);

    message_entry_t current_msg;

    while (upMessageQueue.size())
    {
        upMessageQueue.pop(&current_msg);
        delete current_msg.msg;
    }

    while (downMessageQueue.size())
    {
        downMessageQueue.pop(&current_msg);
        delete current_msg.msg;
    }

    sem_destroy(&ipc_sem);
}

void* Node::do_listen_thread(void *arg)
{
    Node *me = (Node *) arg;
    int port;
    int maxDescriptor = -1;

    me->listen_thread_is_running = true;

    // Create listening sockets
    for (port = 0; port < MAX_PORTS; ++port)
    {
        me->sktList[port] = me->CreateListeningSocket(me->portList[port]);
        if (me->sktList[port] > maxDescriptor)
            maxDescriptor = me->sktList[port];
    }

    // Init select() parameters
    fd_set sockSet;
    struct timeval selTimeout;

    // Thread loop
    while (!me->die)
    {
        FD_ZERO(&sockSet);

        for (port = 0; port < MAX_PORTS; ++port)
            FD_SET(me->sktList[port], &sockSet);

        selTimeout.tv_sec = me->selectTimeout;
        selTimeout.tv_usec = 0;

        if (select(maxDescriptor + 1, &sockSet, NULL, NULL, &selTimeout) == 0)
            me->log.print(LOG_PARANOID, "Select timeout (%ld) - still alive\n", me->selectTimeout);
        else
        {
            for (port = 0; port < MAX_PORTS; ++port)
                if (FD_ISSET(me->sktList[port], &sockSet))
                {
                    me->log.print(LOG_INFO, "[ ] Request on port %d\n",
                                  me->portList[port]);
                    me->HandleTCPRequest(me->AcceptTCPConnection(me->sktList[port]),
                                         me->portList[port]);
                }
        }
    }

    // Close listening sockets
    for (port = 0; port < MAX_PORTS; ++port)
    {
        if (close(me->sktList[port]) < 0)
            me->log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n",
                          me->sktList[port], strerror(errno));
        else
            me->log.print(LOG_PARANOID, "[x] Socket %d\n", me->sktList[port]);
    }

    me->listen_thread_is_running = false;
    me->log.print(LOG_DEBUG, "[x] Listen thread\n");
    pthread_exit(0);
}

void* Node::do_process_thread(void *arg)
{
    Node *me = (Node *) arg;
    me->process_thread_is_running = true;

    while (!me->die)
    {
        // Wait for messages
        sem_wait(&me->ipc_sem);

        // Process messages from upper nodes
        message_entry_t new_message;

        if (me->downMessageQueue.pop(&new_message) == 0)
        {
            me->processDownMessage(new_message.msg, new_message.socket);
            delete new_message.msg;
        }

        // Process messages from lower nodes
        if (me->upMessageQueue.pop(&new_message) == 0)
        {
            me->processUpMessage(new_message.msg, new_message.socket);
            delete new_message.msg;
        }
    }

    me->process_thread_is_running = false;
    me->log.print(LOG_DEBUG, "[x] Process thread\n");
    pthread_exit(0);
}

int Node::CreateListeningSocket(unsigned short port)
{
    int newSkt;
    if ((newSkt = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Error in socket() : %s\n", strerror(errno));
        exit(1);
    }
    log.print(LOG_PARANOID, "[o] Socket %d\n", newSkt);

    int yes = 1;
    setsockopt(newSkt, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in nodeAddr;
    memset(&nodeAddr, 0, sizeof(nodeAddr));
    nodeAddr.sin_family = AF_INET;
    nodeAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    nodeAddr.sin_port = htons(port);

    if (bind(newSkt, (struct sockaddr *) &nodeAddr, sizeof(nodeAddr)) < 0)
    {
        log.print(LOG_ERROR, "[!] Error in bind(%d) : %s\n", port, strerror(errno));
        exit(1);
    }

    if (listen(newSkt, MAX_MSG_QUEUE) < 0)
    {
        log.print(LOG_ERROR, "[!] Error in listen() : %s\n", strerror(errno));
        exit(1);
    }

    log.print(LOG_PARANOID, "[ ] Created listening socket: %d\n", newSkt);

    return newSkt;
}

int Node::AcceptTCPConnection(int sock)
{
    struct sockaddr_in clientAddr;
    unsigned int clientLen = sizeof(clientAddr);

    int clientSock;
    if ((clientSock = accept(sock, (struct sockaddr *) & clientAddr, &clientLen))
            < 0)
    {
        log.print(LOG_ERROR, "[!] Error in accept() : %s\n", strerror(errno));
        return -1;
    }

    log.print(LOG_PARANOID, "[o] Socket %d\n", clientSock);

    return clientSock;
}

void Node::HandleTCPRequest(int clientSock, unsigned short port)
{
    log.print(LOG_PARANOID, "[ ] Handling client socket: %d\n", clientSock);

    Message *msg = new Message();
    msg->Recv(clientSock);

    message_entry_t new_message;
    new_message.socket = clientSock;
    new_message.msg = msg;

    if (port == portList[UP_PORT])
        upMessageQueue.push(new_message);
    else
        downMessageQueue.push(new_message);

    // Notify the 'process_thread'
    sem_post(&ipc_sem);
}

void Node::setTimeout(long timeout)
{
    selectTimeout = timeout;
}

timestamp_t Node::getTimestamp()
{
#if !defined(__APPLE__)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long) (ts.tv_sec * 1E9 + ts.tv_nsec);
#else

    return mach_absolute_time();
#endif
}

string Node::getName()
{
    return name;
}

string Node::getIpAddress()
{
    return ip;
}

unsigned short Node::getUpPort()
{
    return portList[UP_PORT];
}

unsigned short Node::getDownPort()
{
    return portList[DOWN_PORT];
}

bool Node::parseAddress(string *nodeAddress, unsigned int *nodePort)
{
    bool retval = false;

    list<string> tokens;
    string::size_type lastPos = nodeAddress->find_first_not_of(":", 0);
    string::size_type pos = nodeAddress->find_first_of(":", 0);

    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(nodeAddress->substr(lastPos, pos - lastPos));
        lastPos = nodeAddress->find_first_not_of(":", pos);
        pos = nodeAddress->find_first_of(":", lastPos);
    }

    if (tokens.size() == 2)
    {
        *nodeAddress = tokens.front();
        tokens.pop_front();
        *nodePort = atoi(tokens.front().c_str());

        retval = true;
    }

    return retval;
}

bool Node::parseNameAndAddress(string *nodeNickname, string *nodeAddress, unsigned int *nodePort)
{
    bool retval = false;

    list<string> tokens;
    string::size_type lastPos = nodeAddress->find_first_not_of(":", 0);
    string::size_type pos = nodeAddress->find_first_of(":", 0);

    while (string::npos != pos || string::npos != lastPos)
    {
        tokens.push_back(nodeAddress->substr(lastPos, pos - lastPos));
        lastPos = nodeAddress->find_first_not_of(":", pos);
        pos = nodeAddress->find_first_of(":", lastPos);
    }

    if (tokens.size() == 3)
    {
        *nodeNickname = tokens.front();
        tokens.pop_front();
        *nodeAddress = tokens.front();
        tokens.pop_front();
        *nodePort = atoi(tokens.front().c_str());

        retval = true;
    }

    return retval;
}


int Node::getLocalIPAddress(string &ip_addr, string iface)
{
    int sockfd, size = 1;
    struct ifreq *ifr;
    struct ifconf ifc;
    struct sockaddr_in sa;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0)
    {
        log.print(LOG_ERROR, "[!] Cannot open socket: %s\n", strerror(errno));
        return -1;
    }

    ifc.ifc_len = IFRSIZE;
    ifc.ifc_req = NULL;

    do
    {
        ++size;

        // Realloc buffer size until no overflow occurs
        ifc.ifc_req = (struct ifreq *)realloc((void *)(ifc.ifc_req), IFRSIZE);
        if (ifc.ifc_req == NULL)
        {
            log.print(LOG_ERROR, "[!] Out of memory\n");
            return -2;
        }

        ifc.ifc_len = IFRSIZE;

        if (ioctl(sockfd, SIOCGIFCONF, &ifc))
        {
            log.print(LOG_ERROR, "[!] ioctl SIOCFIFCONF\n");
            return -3;
        }
    }
    while (IFRSIZE <= ifc.ifc_len);

    ifr = ifc.ifc_req;

    for (;(char *) ifr < (char *) ifc.ifc_req + ifc.ifc_len; ++ifr)
    {
        if (ifr->ifr_addr.sa_data == (ifr + 1)->ifr_addr.sa_data)
        {
            continue;  // duplicate, skip it
        }

        if (ioctl(sockfd, SIOCGIFFLAGS, ifr))
        {
            continue;  // failed to get flags, skip it
        }

        if (strncmp(ifr->ifr_name, iface.c_str(), iface.length()) == 0)
        {
            ip_addr = string(inet_ntoa(inaddrr(ifr_addr.sa_data)));
            log.print(LOG_PARANOID, "[ ] Found: %s", ip_addr.c_str());
            break;
        }
    }

    close(sockfd);
    return 0;
}
