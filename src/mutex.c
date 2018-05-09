/*
 * mutex.c
 *
 *  Created on: 8 may. 2018
 *      Author: Alejo
 */

#include <pthread.h>

static pthread_mutex_t piMutexes [10] ;

void lock (int key)
{
  pthread_mutex_lock(&piMutexes[key]) ;
}

void unlock (int key)
{
  pthread_mutex_unlock(&piMutexes[key]) ;
}

