/*
 * timers.h
 *
 *  Created on: 23 feb. 2018
 *      Author: Alejo
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include <time.h>
#include <signal.h>


typedef enum
{
	STATUS_OK,
	STATUS_ERROR
}timerState;

timerState state;

/*Prototipos*/
timerState timerIdInit(timer_t *timerId, void* func);
timerState timerIdStart(timer_t *timerId, int ms);
timerState timerIdStop(timer_t *timerId);

#endif /* TIMERS_H_ */
