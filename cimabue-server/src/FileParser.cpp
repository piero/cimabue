/*
* FileParser.cpp
*
*  Created on: 26 Aug 2009
*      Author: piero
*/

#include "FileParser.h"

using namespace std;


FileParser::FileParser(string filename)
		: file(filename)
{}

FileParser::~FileParser()
{}

int FileParser::parse()
{
	return manageFile();
}

int FileParser::manageFile()
{
	int retval = 0;

	// Open file
	FILE *fs = fopen(file, "r");
	if (fs == NULL)
	{
		log.print(LOG_ERROR, "[!] Error opening file %s: %s\n, file.c_str(), strerror(errno)");
		retval = -1;
		return retval;
	}

	log.print(LOG_DEBUG, "[ ] File %s: open\n", file.c_str());

	// Get file size
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	log.print(LOG_DEBUG, "[ ] File %s: size is %ld\n", file.c_str(), file_size);

	// Allocate memory to contain the whole file
	char *buffer = (char *)malloc(sizeof(char) * file_size);
	if (buffer == NULL)
	{
		log.print(LOG_ERROR, "[!] Error allocating memory: %s\n, strerror(errno)");
		retval = -2;
		return retval;
	}

	log.print(LOG_DEBUG, "[ ] File %s: allocated memory\n", file.c_str());

	// Copy the file into memory
	size_t bytes = fread(buffer, 1, file_size, file);
	if (bytes != file_size)
	{
		log.print(LOG_ERROR, "[!] Error copying file into memory: %s\n, strerror(errno)");
		free(buffer);
		retval = -3;
		return retval;
	}

	log.print(LOG_DEBUG, "[ ] File %s: copied %d bytes into memory\n", file.c_str(), bytes);

	// Parse file
	retval = parseFile();

	// Free allocated memory
	free(buffer);
	log.print(LOG_DEBUG, "[ ] File %s: released memory\n", file.c_str());

	// Close file
	if (fclose(fs) != 0)
	{
		log.print(LOG_ERROR, "[!] Error closing file %s: %s\n", file.c_str(), strerror(errno));
		retval = -5;
	}

	log.print(LOG_DEBUG, "[ ] File %s: close\n", file.c_str());

	return retval;
}

int FileParser::parseFile()
{
	int ret = 0;

	// TODO TODO TODO TODO TODO TODO TODO

	return ret;
}

map<std::string, unsigned short> FileParser::getServerList()
{
	return server_list;
}
