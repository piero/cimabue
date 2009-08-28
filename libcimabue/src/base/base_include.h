/*
* base_include.h
*
*  Created on: May 4, 2009
*      Author: piero
*/

#ifndef BASE_INCLUDE_H_
#define BASE_INCLUDE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <string>
#include <list>
#include <pthread.h>
#include <semaphore.h>
#include "../log/Log.h"
#include "sync.h"

enum
{
    RET_SUCCESS,
    RET_ERROR
};

#endif /* BASE_INCLUDE_H_ */
