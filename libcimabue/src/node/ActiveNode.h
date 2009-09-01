/*
* ActiveNode.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef ACTIVENODE_H_
#define ACTIVENODE_H_

#include "Node.h"
#include <map>
#include "../message/MessageQueue.h"
#include "../message/AddClientMessage.h"
#include "../message/ErrorMessage.h"
#include "../message/InformMessage.h"
#include "../message/PingMessage.h"
#include "../message/SendMessage.h"
#include "../message/SubscribeMessage.h"
#include "../message/UnsubscribeMessage.h"

#if !defined(__APPLE__)
#	include <net/if.h>
#	include <sys/socket.h>
#	include <net/if_arp.h>
#endif

#include <sys/ioctl.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <stdint.h>
#include <mach/mach_time.h>
#else
#include <time.h>
#endif

#include "../message/Message.h"
#include "../message/ErrorMessage.h"


// Necessary to find our IP address
#define inaddrr(x) (*(struct in_addr *) &ifr->x[sizeof sa.sin_port])
#define IFRSIZE   ((int)(size * sizeof (struct ifreq)))

// Max number of listening ports
#define MAX_PORTS	1

// Max length of message queue (incoming messages are discarded after that)
#define MAX_MSG_QUEUE	5


class ActiveNode : public Node
{
public:
    ActiveNode(unsigned short port, long select_timeout = 5);
    virtual ~ActiveNode();

    // Accessors
    std::string getIpAddress();
    unsigned short getPort();
    void setTimeout(long timeout);

    // TODO: Make it protected
    virtual int processMessage(Message *msg, int skt) = 0;

protected:

    // Parse nicknane and Node name
    bool parseNicknameAndName(std::string *nickname, std::string *node_name);

    // Extract Nickname from nodeData
    std::string parseNickname(std::string nodeData);

    // Extract IP and PORT from nodeAddress
    // and put the result in nodeAddress (overwriting it) and listenPort
    bool parseAddress(std::string *nodeAddress, unsigned int *listenPort);

    // Extract nickname, IP and Port from nodeAddress
    // and put the result in nodeNickname, nodeAddress (overwriting it) and listenPort
    bool parseNicknameAndAddress(std::string *nodeNickname, std::string *nodeAddress, unsigned int *listenPort);

    static void* do_listen_thread(void *arg);
    static void* do_process_thread(void *arg);

    int CreateListeningSocket(unsigned short port);
    int AcceptTCPConnection(int sock);
    void HandleTCPRequest(int clientSock, unsigned short port);

    int getLocalIPAddress(std::string &ip_addr, std::string iface);
    bool tokenizeClientData(std::list<std::string> *tokens, std::string *data);

    std::string ip;
    unsigned short listenPort;

    // Listening socket
    int listenSocket;

    // Message queues
    MessageQueue messageQueue;

    sem_t ipc_sem;
    pthread_t listen_thread;
    pthread_t process_thread;
    bool listen_thread_is_running;
    bool process_thread_is_running;
    bool die;

    long selectTimeout;


    // Return codes
    enum
    {
        NODE_RET_SUCCESS,
        NODE_RET_ERROR,
        NODE_RET_DESTINATION_IS_NOT_ME,
        NODE_RET_CLIENT_NOT_FOUND,
        NODE_RET_SERVER_NOT_FOUND
    };

    // Node default ports
    enum
    {
        NODE_PORT_SERVER_UP = 8001,
        NODE_PORT_SERVER_DOWN = 8000,
        NODE_PORT_CLIENT_UP = 6001,
        NODE_PORT_CLIENT_DOWN = 6000
    };
};

#endif /* ACTIVENODE_H_ */
