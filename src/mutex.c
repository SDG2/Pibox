/*
 * mutex.c
 *
 *  Created on: 8 may. 2018
 *      Author: Alejo
 */

#include <pthread.h>

static pthread_mutex_t piMutexes [10] ;

/**
 * @brief Bloquea el hilo actual
 * @param key inidice del hilo a bloquear
 */
void lock (int key)
{
  pthread_mutex_lock(&piMutexes[key]) ;
}
/**
 * @brief desloquea el hilo actual
 * @param key inidice del hilo a desbloquear
 */

void unlock (int key)
{
  pthread_mutex_unlock(&piMutexes[key]) ;
}

