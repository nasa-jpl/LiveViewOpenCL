
#ifndef _EDT_OS_LNX_H_
#define _EDT_OS_LNX_H_



typedef int HANDLE;

#define DIRHANDLE caddr_t
#define NULL_HANDLE ((HANDLE)-1)

typedef unsigned int uint_t;
typedef unsigned char uchar_t;
typedef unsigned short ushort_t;

#ifndef __KERNEL__

#ifndef _GNU_SOURCE

#define _GNU_SOURCE

#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#include <linux/ioctl.h>

#include <pthread.h>

#include <semaphore.h>

typedef pthread_t thread_t;

#include <stdlib.h>

#include <pthread.h>
#include <sys/user.h>

#include <limits.h>

#define WINAPI

#define ASSERT(f) ((void)0)

/* utility helpers */

#ifndef TRUE

#define TRUE (1)
#define FALSE (0)

#endif

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef unsigned long ULONG;

typedef off_t EdtFileOffset;


#define INFINITE 0

#define PAGESIZE PAGE_SIZE
#define SECTOR_SIZE PAGE_SIZE

#define _MAX_PATH _POSIX_PATH_MAX
#define MAX_PATH _POSIX_PATH_MAX

#define THREAD_RETURN void *
#define THREAD_FUNC_MODIFIER
#define THREAD_FUNC_DECLARE THREAD_RETURN
typedef void * (*edt_thread_function)(void *);

#define LaunchThread(pThread,func,pointer) \
               pthread_create(&pThread,NULL,func,pointer);

#define WaitForThread(pThread,timeout) \
{ int return; pthread_join(pThread, &return;}

#define CRITICAL_SECTION pthread_mutex_t

typedef pthread_mutex_t mutex_t;


#define EnterCriticalSection(lock) pthread_mutex_lock(lock)
#define LeaveCriticalSection(lock) pthread_mutex_unlock(lock)
#define DeleteCriticalSection(lock) pthread_mutex_destroy(lock)
#define InitializeCriticalSection(lock) pthread_mutex_init(lock, NULL)


#define create_mutex_named(nm)  pthread_mutex_init(&m, NULL)
#define create_mutex(m)    pthread_mutex_init(&m, NULL)
#define delete_mutex(m)    pthread_mutex_destroy(&m)

#define wait_mutex_timeout(m,timeout) WaitForSingleObject(m,timeout)
#define wait_mutex(m) pthread_mutex_lock(&m)
#define release_mutex(m)    pthread_mutex_unlock(&m)


typedef sem_t event_t;

#define create_event(evt, manual, init_state) \
    sem_init(&evt,0,init_state)

#define wait_on_event_timeout(m,timeout) edt_wait_event_timeout(&m,timeout)
#define wait_on_event(m) sem_wait(&m )

#define signal_event(e) sem_post(&e)

typedef thread_t thread_type;

#ifdef _DEBUG

#define TRACE printf

#else

#define TRACE

#endif

int
edt_wait_event_timeout(sem_t *p, int timeout);

#else

typedef HANDLE thread_t;

#endif


#endif

