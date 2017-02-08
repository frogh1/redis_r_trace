

/* spinlock.h*/

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

typedef struct _tagSPINLOCK
{
	pthread_mutex_t Mutex;
}SPINLOCK_S;



#define InitSpinLock(x) pthread_mutex_init(&(x)->Mutex,NULL)


#define SpinLock_Lock(x) pthread_mutex_lock(&(x)->Mutex)

#define SpinLock_unLock(x) pthread_mutex_unlock(&(x)->Mutex)

#define DestroySpinLock(x) pthread_mutex_destroy(&(x)->Mutex)

#define SpinLock_tryLock(x) pthread_mutex_trylock(&(x)->Mutex)

#endif




