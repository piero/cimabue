/*
* Log.cpp
*
*  Created on: Feb 27, 2009
*      Author: piero
*/

#include "Log.h"


Log::Log(log_level_t default_level)
{
	level = default_level;
}

Log::~Log()
{}


log_level_t Log::level = LOG_DEBUG;

void Log::setLogLevel(log_level_t lvl)
{
	level = lvl;
}


log_level_t Log::getLogLevel()
{
	return level;
}


void Log::print(log_level_t lvl, const char *fmt, ...)
{
	if (fmt != NULL && lvl >= level)
	{
		const char *offset = fmt;
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, offset, args);
		va_end(args);
	}
}

