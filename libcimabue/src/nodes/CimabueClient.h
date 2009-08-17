/*
* CimabueClient.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef CIMABUECLIENT_H_
#define CIMABUECLIENT_H_

#include "Node.h"

#define DATA_SEPARATOR	"@"

class CimabueClient : public Node
{
public:
	CimabueClient(std::string nick, std::string proxy_address, unsigned short port = NODE_PORT_CLIENT_UP);
	virtual ~CimabueClient();

	int sendMessage(std::string dest, std::string content);

	void setProxyIP(std::string ip);
	std::string getProxyIP();

	std::string getNickname();

private:
	static void* do_listen(void *myself);
	int processUpMessage(Message *msg, int skt);
	int processDownMessage(Message *msg, int skt);

	void extractNicknameAndData(std::string s, std::string &nick, std::string &data);

	void connectToProxy(std::string proxyIP);

	bool connectedToProxy;

	std::string nickname;
	std::string proxy;
	std::string proxy_ip;
};


#endif /* CIMABUECLIENT_H_ */
