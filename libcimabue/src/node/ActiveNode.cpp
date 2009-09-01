/*
* Node.cpp
*
*  Created on: May 4, 2009
*      Author: piero
*/

#include "ActiveNode.h"

using namespace std;

ActiveNode::ActiveNode(unsigned short port,
                       long select_timeout)
		: Node()
{
	listenPort = port;
	listen_thread_is_running = false;
	process_thread_is_running = false;
	die = false;
	selectTimeout = select_timeout;

	getLocalIPAddress(ip, "eth0");

#if defined(__APPLE__) && defined(__MACH__)
	// TODO: http://kevincathey.com/code/using-counting-semaphores-on-mac-os-x/
	// TODO: OR USE NAMED SEMAPHORES (easier, maybe)
#else

	sem_init(&ipc_sem, 0, 0);
#endif



	// The double-threaded structure gives us async requests
	pthread_create(&process_thread, NULL, do_process_thread, this);
	pthread_create(&listen_thread, NULL, do_listen_thread, this);

	log.print(LOG_INFO, "[ ] Created node %s (%s:%d)\n",
	          name.c_str(), ip.c_str(), port);
}

ActiveNode::~ActiveNode()
{
	die = true;

	pthread_join(listen_thread, NULL);
	pthread_join(process_thread, NULL);

	message_entry_t current_msg;

	while (messageQueue.size())
	{
		messageQueue.pop(&current_msg);
		delete current_msg.msg;
	}

	sem_destroy(&ipc_sem);
}

void* ActiveNode::do_listen_thread(void *arg)
{
	ActiveNode *me = (ActiveNode *) arg;
	int maxDescriptor = -1;

	me->listen_thread_is_running = true;

	// Create listening sockets
	me->listenSocket = me->CreateListeningSocket(me->listenPort);

	if (me->listenSocket > maxDescriptor)
		maxDescriptor = me->listenSocket;

	// Init select() parameters
	fd_set sockSet;
	struct timeval selTimeout;

	// Thread loop
	while (!me->die)
	{
		FD_ZERO(&sockSet);
		FD_SET(me->listenSocket, &sockSet);

		selTimeout.tv_sec = me->selectTimeout;
		selTimeout.tv_usec = 0;

		if (select(maxDescriptor + 1, &sockSet, NULL, NULL, &selTimeout) == 0)
			me->log.print(LOG_PARANOID, "Select timeout (%ld) - still alive\n", me->selectTimeout);
		else
		{
			if (FD_ISSET(me->listenSocket, &sockSet))
			{
				me->log.print(LOG_PARANOID, "[ ] Request on port %d\n", me->listenPort);
				me->HandleTCPRequest(me->AcceptTCPConnection(me->listenSocket), me->listenPort);
			}
		}
	}

	// Close listening sockets
	if (close(me->listenSocket) < 0)
		me->log.print(LOG_ERROR, "[!] Error closing socket %d: %s\n",
		              me->listenSocket, strerror(errno));
	else
		me->log.print(LOG_PARANOID, "[x] Socket %d\n", me->listenSocket);

	me->listen_thread_is_running = false;
	me->log.print(LOG_PARANOID, "[x] Listen thread\n");
	pthread_exit(0);
}

void* ActiveNode::do_process_thread(void *arg)
{
	ActiveNode *me = (ActiveNode *) arg;
	me->process_thread_is_running = true;

	while (!me->die)
	{
		// Wait for messages
		sem_wait(&me->ipc_sem);

		// Process messages from upper nodes
		message_entry_t new_message;

		// Process messages from lower nodes
		if (me->messageQueue.pop(&new_message) == 0)
		{
			me->processMessage(new_message.msg, new_message.socket);
			delete new_message.msg;
		}
	}

	me->process_thread_is_running = false;
	me->log.print(LOG_PARANOID, "[x] Process thread\n");
	pthread_exit(0);
}

int ActiveNode::CreateListeningSocket(unsigned short port)
{
	int newSkt;

	log.print(LOG_PARANOID, "[ ] Creating socket on port %d...\n", port);

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

int ActiveNode::AcceptTCPConnection(int sock)
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

void ActiveNode::HandleTCPRequest(int clientSock, unsigned short port)
{
	log.print(LOG_PARANOID, "[ ] Handling client socket: %d\n", clientSock);

	Message *msg = new Message();
	msg->Recv(clientSock);

	message_entry_t new_message;
	new_message.socket = clientSock;
	new_message.msg = msg;

	if (port == listenPort)
	{
		messageQueue.push(new_message);

		// Notify the 'process_thread'
		sem_post(&ipc_sem);
	}
}

void ActiveNode::setTimeout(long timeout)
{
	selectTimeout = timeout;
}

string ActiveNode::getIpAddress()
{
	return ip;
}

unsigned short ActiveNode::getPort()
{
	return listenPort;
}

string ActiveNode::parseNickname(string nodeData)
{
	string nickname = "";

	list<string> tokens;

	if (tokenizeClientData(&tokens, &nodeData))
	{
		nickname = tokens.front();
	}

	return nickname;
}

bool ActiveNode::parseNicknameAndName(string *nickname, string *node_name)
{
	bool retval = false;

	list<string> tokens;

	if (tokenizeClientData(&tokens, nickname))
	{
		*nickname = tokens.front();
		tokens.pop_front();

		*node_name = tokens.front();

		retval = true;
	}

	return retval;
}

bool ActiveNode::parseAddress(string *nodeAddress, unsigned int *listenPort)
{
	bool retval = false;

	list<string> tokens;

	if (tokenizeClientData(&tokens, nodeAddress))
	{
		tokens.pop_front();	// Discard nickname

		*nodeAddress = tokens.front();
		tokens.pop_front();

		*listenPort = atoi(tokens.front().c_str());

		retval = true;
	}

	return retval;
}

bool ActiveNode::parseNicknameAndAddress(string *nodeNickname, string *nodeAddress, unsigned int *listenPort)
{
	bool retval = false;

	list<string> tokens;

	if (tokenizeClientData(&tokens, nodeAddress))
	{
		*nodeNickname = tokens.front();
		tokens.pop_front();

		*nodeAddress = tokens.front();
		tokens.pop_front();

		*listenPort = atoi(tokens.front().c_str());

		retval = true;
	}

	return retval;
}

bool ActiveNode::tokenizeClientData(list<string> *tokens, string *data)
{
	string::size_type lastPos = data->find_first_not_of(":", 0);
	string::size_type pos = data->find_first_of(":", 0);

	while (string::npos != pos || string::npos != lastPos)
	{
		tokens->push_back(data->substr(lastPos, pos - lastPos));
		lastPos = data->find_first_not_of(":", pos);
		pos = data->find_first_of(":", lastPos);
	}

	if (tokens->size() > 0)
		return true;
	else
		return false;
}

int ActiveNode::getLocalIPAddress(string &ip_addr, string iface)
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
			log.print(LOG_PARANOID, "[ ] Found: %s\n", ip_addr.c_str());
			break;
		}
	}

	close(sockfd);
	return 0;
}