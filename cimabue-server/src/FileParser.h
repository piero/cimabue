/*
* FileParser.h
*
*  Created on: 26 Aug 2009
*      Author: piero
*/

#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <map>
#include <log/Log.h>


class FileParser
{
public:
	FileParser(std::string filename);
	virtual ~FileParser();

	int parse();
	std::map<std::string, unsigned short> getServerList();

protected:
	int manageFile();
	int parseFile();

	std::string file;
	std::map<std::string, unsigned short> server_list;

	Log log;
};


#endif /* FILEPARSER_H_ */
