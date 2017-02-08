
/* thread.h */

#ifndef _THREAD_H_
#define _THREAD_H_


typedef struct _tagTHREAD_HANDLE
{
	pthread_t Thread;
}THREAD_HANDLE;

#include <pthread.h>

#define __CreateThread(handle,Thread_Proc,arg,ret) \
{\
	ret = pthread_create(&handle.Thread, NULL, Thread_Proc, arg);\
}
#define __WaitThreadExit(handle) pthread_join(handle.Thread, NULL)





#endif



