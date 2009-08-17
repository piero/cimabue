/*
* Node.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef NODE_H_
#define NODE_H_

#include <stdlib.h>
#include <map>
#include "../message/MessageQueue.h"

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

#include "../base/base_include.h"
#include "../message/Message.h"
#include "../message/ErrorMessage.h"

#if !defined(__APPLE__)
typedef long timestamp_t;
#else
typedef uint64_t timestamp_t;
#endif

// Necessary to find our IP address
#define inaddrr(x) (*(struct in_addr *) &ifr->x[sizeof sa.sin_port])
#define IFRSIZE   ((int)(size * sizeof (struct ifreq)))

// Max number of listening ports
#define MAX_PORTS	2

// Max length of message queue (incoming messages are discarded after that)
#define MAX_MSG_QUEUE	5

#define UP_PORT		0
#define DOWN_PORT	1

class Node
{
public:
    Node(unsigned short upPort, unsigned short downPort, long timeout = 5);
    virtual ~Node();

    std::string getName();
    std::string getIpAddress();
    unsigned short getUpPort();
    unsigned short getDownPort();

    virtual int processUpMessage(Message *msg, int skt) = 0;
    virtual int processDownMessage(Message *msg, int skt) = 0;

    void setTimeout(long timeout);
    timestamp_t getTimestamp();

protected:

    // Extract IP and PORT from nodeAddress
    // and put the result in nodeAddress (overwriting it) and nodePort
    bool parseAddress(std::string *nodeAddress, unsigned int *nodePort);

    // Extract nickname, IP and Port from nodeAddress
    // and put the result in nodeNickname, nodeAddress (overwriting it) and nodePort
    bool parseNameAndAddress(std::string *nodeNickname, std::string *nodeAddress, unsigned int *nodePort);

    static void* do_listen_thread(void *arg);
    static void* do_process_thread(void *arg);

    int CreateListeningSocket(unsigned short port);
    int AcceptTCPConnection(int sock);
    void HandleTCPRequest(int clientSock, unsigned short port);

    int getLocalIPAddress(std::string &ip_addr, std::string iface);

    std::string name;
    std::string ip;

    // Arrays of listening ports and sockets, used by select()
    unsigned short portList[MAX_PORTS];
    int sktList[MAX_PORTS];

    // Message queues
    MessageQueue upMessageQueue;
    MessageQueue downMessageQueue;

    sem_t ipc_sem;
    pthread_t listen_thread;
    pthread_t process_thread;
    bool listen_thread_is_running;
    bool process_thread_is_running;
    bool die;

    long selectTimeout;

    Log log;


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

#endif /* NODE_H_ */
