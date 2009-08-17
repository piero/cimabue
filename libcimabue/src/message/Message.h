/*
* Message.h
*
*  Created on: Apr 3, 2009
*      Author: piero
*/

#ifndef MESSAGE_H_
#define MESSAGE_H_


#include "../base/base_include.h"


#define MSG_BUFFER_SIZE		4096

/**
 * Describes a generic Message
 */

typedef enum
{
    MSG_NONE = 0,
    MSG_ADD_CLIENT,        	// add a client
    MSG_REM_CLIENT,        	// remove a client
    MSG_SEND_MESSAGE,       // send a message
    MSG_GET_CLIENT_LIST,    // a proxy or a server requests client list from server
    MSG_SET_CLIENT_LIST,    // send client list (server)
    MSG_PING_CLIENT,		// Ping a client to verify its connection
    MSG_USER,				// user-specific
    MSG_ERROR
} message_t;


typedef enum
{
    MSG_PRIORITY_LOW = 1,
    MSG_PRIORITY_NORMAL,
    MSG_PRIORITY_HIGH,
    MSG_PRIORITY_CRITICAL
} message_priority_t;


#define MSG_SEPARATOR	"-"
#define MSG_VOID		" "
#define MSG_END			"|"


/*
 * Base message format
 *
 * type-src_client-dst_client-src_proxy-dst_proxy-src_server-dst_server-data-priority
 *
 * (8 fields)
 *
 */

#define MSG_FIELDS	9

class Message
{
public:
	Message();

	Message(std::string msg);

	Message(message_t msg_type,
	        std::string fromClient,
	        std::string toClient,
	        std::string fromServer,
	        std::string toServer,
	        std::string content,
	        message_priority_t msg_priority = MSG_PRIORITY_NORMAL);

	virtual ~Message();

	//virtual int Recv(std::string ip, unsigned int port);	// receive a message
	virtual Message* Send(std::string ip, unsigned int port);
	virtual int Recv(int skt);	// receive a message
	virtual int Reply(int skt);	// reply to a message

	// Type
	virtual message_t getType();
	virtual void setType(message_t t);
	virtual std::string typeToString(message_t type);

	// Client Source
	virtual std::string getClientSource();
	virtual void setClientSource(std::string s);

	// Client Dest
	virtual std::string getClientDest();
	virtual void setClientDest(std::string d);

	// Server Source
	virtual std::string getServerSource();
	virtual void setServerSource(std::string s);

	// Server Dest
	virtual std::string getServerDest();
	virtual void setServerDest(std::string d);

	// Data
	virtual std::string getData();
	virtual void setData(std::string d);

	// Priority
	virtual message_priority_t getPriority();
	virtual void setPriority(message_priority_t p);

	virtual void copy(Message &to);

	virtual std::string serialize();
	virtual void deserialize(std::string s);

	virtual void dump();	// dump a message to stdout

	virtual bool isErrorMessage();

protected:
	//std::string separator;

	// Message fields
	message_t type; 			// Message type
	std::string clientSrc;		 	// Message source
	std::string clientDest;			// Message destination
	std::string serverSrc;
	std::string serverDest;
	std::string data;			// Message content
	message_priority_t priority; // Message priority

	double id;

	Log messageLog;

	virtual message_t encodeType(std::string s);
	virtual std::string decodeType(message_t t);

	virtual message_priority_t encodePriority(std::string s);
	virtual std::string decodePriority(message_priority_t p);
};

#endif /* MESSAGE_H_ */
