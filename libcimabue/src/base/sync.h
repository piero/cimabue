#ifndef SYNC_H_
#define SYNC_H_
/*
 **  Provision Communications Ltd
 **  Date:   10/12/2007
 **  Author: Mike Dyer
 **
 **  Description: Synchronisation abstraction
 **
 */

// PORT! -- These classes will require porting for various platforms

#ifdef __linux__
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#elif defined __TIDSP__
#include <std.h>
#include <sem.h>
#include <lck.h>

#elif defined _WIN32_WCE
#include "winCE_defines.h"

#elif defined WIN32
#include "win32_defines.h"

#elif defined(__APPLE__) && defined(__MACH__)
#include <pthread.h>
#include <semaphore.h>

#else
#error PORT YOUR SYNC CLASS
#endif

class portable_cond_var;

/*!
 * \brief The portable mutex class.
 *
 * A mutex class that works on Linux, Windows and TI DSP, Oh My!
 */
class portable_mutex
{
public:
	portable_mutex();
	~portable_mutex();

	bool lock(); //!< Lock the mutex
	//!< \return true if lock worked

	bool trylock(); //!< Try and lock the mutex (does not block)
	//!< \return true if lock worked

	bool unlock(); //!< Unlock the mutex
	//!< \return true if unlock worked

	friend class portable_cond_var;
private:
#ifdef __linux__
	pthread_mutex_t mutex; //!< pthread mutex structure

#elif defined(__APPLE__) && defined(__MACH__)
	pthread_mutex_t mutex;

#elif defined __TIDSP__
	LCK_Attrs attrs;
	LCK_Obj *mutex;
#elif defined WIN32 || defined _WIN32_WCE
	HANDLE mutex;
#       define MAX_SEM_COUNT 10
#endif
	bool useable; //!< mutex created and useable

};

inline portable_mutex::portable_mutex()
{
#ifdef __linux__
	useable = (pthread_mutex_init(&mutex, NULL) == 0);

#elif defined(__APPLE__) && defined(__MACH__)
	useable = (pthread_mutex_init(&mutex, NULL) == 0);

#elif defined __TIDSP__
	mutex = LCK_create(&attrs);
	useable = (mutex != NULL);
#elif defined WIN32 || defined _WIN32_WCE
	mutex = CreateMutex(NULL, false, NULL);
	(mutex == NULL) ? useable = false : useable = true;
#endif
}

inline portable_mutex::~portable_mutex()
{
	if (useable)
#ifdef __linux__
		pthread_mutex_destroy(&mutex);
#elif defined(__APPLE__) && defined(__MACH__)
		pthread_mutex_destroy(&mutex);
#elif defined __TIDSP__
	LCK_delete(mutex);
#elif defined WIN32 || defined _WIN32_WCE
	CloseHandle(mutex);
#endif
	useable = false;
}

inline bool portable_mutex::lock()
{
	if (useable)
#ifdef __linux__
		return pthread_mutex_lock(&mutex) == 0;
#elif defined(__APPLE__) && defined(__MACH__)
		return pthread_mutex_lock(&mutex) == 0;
#elif defined __TIDSP__
	return LCK_pend(mutex, SYS_FOREVER);
#elif defined WIN32 || defined _WIN32_WCE
	{
		DWORD dwRet = WaitForSingleObject(mutex, INFINITE);
		switch (dwRet)
		{
			case WAIT_OBJECT_0:
			return true;
			break;

			case WAIT_ABANDONED:
			return false;
			break;

			default:
			return false;
		}
	}
#endif
	else
		return false;
}

inline bool portable_mutex::trylock()
{
	if (useable)
#ifdef __linux__
		return pthread_mutex_trylock(&mutex) == 0;
#elif defined(__APPLE__) && defined(__MACH__)
		return pthread_mutex_trylock(&mutex) == 0;
#elif defined __TIDSP__
	return LCK_pend(mutex, 0);
#elif defined WIN32 || defined _WIN32_WCE
	{
		//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	}
#endif
	else
		return false;
}

inline bool portable_mutex::unlock()
{
	if (useable)
#ifdef __linux__
		return pthread_mutex_unlock(&mutex) == 0;
#elif defined(__APPLE__) && defined(__MACH__)
		return pthread_mutex_unlock(&mutex) == 0;
#elif defined __TIDSP__
	{
		LCK_post(mutex);
		return true;
	}
#elif defined WIN32 || defined _WIN32_WCE
	{
		ReleaseMutex(mutex);
		return true;
	}
#endif
	else
		return false;
}

/*!
 * \brief The portable semaphore class.
 *
 * A semaphore class that works on Linux, Windows and TI DSP, Oh My!
 */

class portable_semaphore
{
public:
	portable_semaphore(int count = 0, int max = 32);
	~portable_semaphore();

	bool post(); //!< post the semaphore
	//!< \return true if successful
	bool pend(); //!< wait for the semaphore to be posted
	//!< \return true if successful
	bool timedpend(void *timestruct); //!< wait with timeout for the semaphore to be posted
	//!< \param timestruct is how long to wait for the semaphpre
	//!< \return true if successful, false if error or timeout
	bool trypend(); //!< test the semaphore (does not block)
	//!< \return true if semaphore pend'ed

	int getcount(); //!< Get the current semaphore count
	//!< \return number of times semaphore post'ed
private:
#ifdef __linux__
	sem_t sem; //!< pthread semaphore struct
#elif defined(__APPLE__) && defined(__MACH__)
	sem_t sem;
#elif defined __TIDSP__
	SEM_Obj *sem;
#elif defined WIN32 || defined _WIN32_WCE
	HANDLE sem;
#endif
	bool useable; //!< semaphore created and useable
};

inline portable_semaphore::portable_semaphore(int count, int max)
{
#ifdef __linux__
	useable = (sem_init(&sem, 0, count) != -1);
#elif defined(__APPLE__) && defined(__MACH__)
	useable = (sem_init(&sem, 0, count) != -1);
#elif defined __TIDSP__
	sem = SEM_create(0, &SEM_ATTRS);
	useable = (sem != NULL);
#elif defined WIN32 || defined _WIN32_WCE
	{
		sem = CreateSemaphore(NULL, count, max, NULL);
		useable = (sem != NULL);
	}
#endif
}

inline portable_semaphore::~portable_semaphore()
{
	if (useable)
#ifdef __linux__
		sem_destroy(&sem);
#elif defined(__APPLE__) && defined(__MACH__)
		sem_destroy(&sem);
#elif defined __TIDSP__
	SEM_delete(sem);
#elif defined WIN32 || defined _WIN32_WCE
	CloseHandle(sem);
#endif
	useable = false;
}

inline bool portable_semaphore::post()
{
	if (useable)
#ifdef __linux__
		return (sem_post(&sem) == 0);
#elif defined(__APPLE__) && defined(__MACH__)
		return (sem_post(&sem) == 0);
#elif defined __TIDSP__
	{
		SEM_post(sem);
		return true;
	}
#elif defined WIN32 || defined _WIN32_WCE
	{
		if (!ReleaseSemaphore(sem, 1, NULL))
		return false;
		return true;
	}
#endif
	else
		return false;
}

inline bool portable_semaphore::pend()
{
	if (useable)
#ifdef __linux__
		return (sem_wait(&sem) == 0);
#elif defined(__APPLE__) && defined(__MACH__)
		return (sem_wait(&sem) == 0);
#elif defined __TIDSP__
	return SEM_pend(sem, SYS_FOREVER);
#elif defined WIN32 || defined _WIN32_WCE
	{
		DWORD dwRet = WaitForSingleObject(sem, INFINITE);
		switch (dwRet)
		{
			case WAIT_OBJECT_0:
			return true;
			break;

			case WAIT_TIMEOUT:
			return false;
			break;

			default:
			return false;
			break;
		}
	}
#endif
	else
		return false;
}

inline bool portable_semaphore::timedpend(void *timestruct)
{
	if (useable)
	{
#ifdef __linux__
		struct timespec timeout;

		if (timestruct != NULL)
		{
			// Calculate timeout time...
			time(&timeout.tv_sec);
			timeout.tv_sec += ((const struct timespec*)timestruct)->tv_sec;
			timeout.tv_nsec = 0;
			return (sem_timedwait(&sem, &timeout) == 0);
		}
		else
		return pend(); // Normal untimed pend
#endif
	}
	return false;
}

inline bool portable_semaphore::trypend()
{
	if (useable)
#ifdef __linux__
		return (sem_trywait(&sem) == 0);
#elif defined(__APPLE__) && defined(__MACH__)
		return (sem_trywait(&sem) == 0);
#elif defined __TIDSP__
	return SEM_pend(sem, 0);
#elif defined WIN32 || defined _WIN32_WCE
	{
		//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	}
#endif
	else
		return false;
}

inline int portable_semaphore::getcount()
{
	int value = -1;

#ifdef __linux__

	if (sem_getvalue(&sem, &value) != 0)
	value = -1;
#elif defined(__APPLE__) && defined(__MACH__)
	if (sem_getvalue(&sem, &value) != 0)
		value = -1;

#elif defined __TIDSP__
#error Port getcount!
#elif defined WIN32 || defined _WIN32_WCE
	{
		//TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	}
#endif
	return value;
}

/*!
 * \brief The portable condition variable class.
 *
 * A condition variable class that works on Linux, Windows and TI DSP, Oh My!
 *
 * You must supply a mutex for this class to work with (and obviously your own condition somewhere too)
 */

class portable_cond_var
{
public:
	portable_cond_var();
	~portable_cond_var();

	bool wait(portable_mutex &m); //!< Wait on the condition
	//!< \param m is the mutex that locks your variable
	//!< \return true if wait OK
	bool broadcast(); //!< Broadcast the condition is met
	//!< \return true if broadcast is OK

private:
	bool useable; //!< The condition variable is created and useable

#ifdef __linux__
	pthread_cond_t cond; //!< The pthread condition variable structure
#elif defined(__APPLE__) && defined(__MACH__)
	pthread_cond_t cond;
#else
#error PORT CONDITION VARIABLES
#endif

};

inline portable_cond_var::portable_cond_var()
{
#ifdef __linux__
	useable = (pthread_cond_init(&cond, NULL) == 0);
#elif defined(__APPLE__) && defined(__MACH__)
	useable = (pthread_cond_init(&cond, NULL) == 0);
#else
#error PORT CONDITION VARIABLES
#endif

}

inline portable_cond_var::~portable_cond_var()
{
	if (useable)
#ifdef __linux__
		pthread_cond_destroy(&cond);
#elif defined(__APPLE__) && defined(__MACH__)
		pthread_cond_destroy(&cond);
#else
#error PORT CONDITION VARIABLES
#endif

}

inline bool portable_cond_var::wait(portable_mutex &m)
{

	if (useable)
	{
#ifdef __linux__
		return (pthread_cond_wait(&cond, &m.mutex) == 0);
#elif defined(__APPLE__) && defined(__MACH__)
		return (pthread_cond_wait(&cond, &m.mutex) == 0);
#else
#error PORT CONDITION VARIABLES
#endif
	}
	else
		return false;
}

inline bool portable_cond_var::broadcast()
{

	if (useable)
	{
#ifdef __linux__
		return (pthread_cond_broadcast(&cond) == 0);
#elif defined(__APPLE__) && defined(__MACH__)
		return (pthread_cond_broadcast(&cond) == 0);
#else
#error PORT CONDITION VARIABLES
#endif
	}
	else
		return false;
}
#endif /*SYNC_H_*/
