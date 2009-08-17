/*
* Log.h
*
*  Created on: Feb 27, 2009
*      Author: piero
*/

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <stdarg.h>


typedef enum
{
	LOG_PARANOID,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
} log_level_t;


class Log
{
public:
	Log(log_level_t default_level = level);
	virtual ~Log();

	void print(log_level_t lvl = level, const char *fmt = NULL, ...);

	static void setLogLevel(log_level_t lvl);
	static log_level_t getLogLevel();

private:
	static log_level_t level;
};


#endif /* LOG_H_ */
