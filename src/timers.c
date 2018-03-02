
#include "timers.h"
#include "kbhit.h" // Para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas
#include "defines.h"
#include "tipos.h"
#include <stdio.h>


timerState timerIdInit(timer_t *timerId, void* func){
	struct sigevent se;

	se.sigev_notify = SIGEV_THREAD;
	se.sigev_value.sival_ptr = timerId; 
	se.sigev_notify_function = func;
	se.sigev_notify_attributes = NULL;
	if(timer_create(CLOCK_MONOTONIC, &se, timerId)	==	-1)
		return STATUS_ERROR;
	return STATUS_OK;

}

timerState timerIdStart(timer_t *timerId, int ms){
	struct itimerspec spec;

	spec.it_value.tv_sec = ms / 1000;
	spec.it_value.tv_nsec = (ms % 1000) * 1000000;
	spec.it_interval.tv_sec = ms / 1000;
	spec.it_interval.tv_nsec = (ms % 1000) * 1000000;

	if(timer_settime(timerId, 0, &spec, NULL) == -1)
		return STATUS_ERROR;
	return STATUS_OK;
}

timerState timerIdDisarm(timer_t *timerId){
	return timerIdStart(timerId,0);
}

timerState timerIdDelete(timer_t *timerId){
	if(timer_delete(timerId) == -1)
		return STATUS_ERROR;
	return STATUS_OK;
}

//CallBack timers
