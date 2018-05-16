/*
 * tmr.c
 *
 *  Created on: 1 de mar. de 2016
 *      Author: Administrador
 */

#include "tmr.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "tipos.h"

void UpdateTimeState();

tmr_t* tmr_new (notify_func_t isr)
{
    tmr_t* this = (tmr_t*) malloc (sizeof (tmr_t));
    tmr_init (this, isr);
    return this;

}

void tmr_init (tmr_t* this, notify_func_t isr) {
    this->se.sigev_notify = SIGEV_THREAD;
    this->se.sigev_value.sival_ptr = &(this->timerid);
    this->se.sigev_notify_function = isr;
    this->se.sigev_notify_attributes = NULL;
    timer_create (CLOCK_REALTIME, &(this->se), &(this->timerid));  /* o CLOCK_MONOTONIC si se soporta */
}

void tmr_destroy(tmr_t* this)
{
    tmr_stop (this);
    free(this);
}

void tmr_startms_period(tmr_t* this, int ms){
		this->spec.it_value.tv_sec = ms / 1000;
	    this->spec.it_value.tv_nsec = (ms % 1000) * 1000000;
	    this->spec.it_interval.tv_sec = ms / 1000;
	    this->spec.it_interval.tv_nsec = (ms % 1000) * 1000000;
	    timer_settime (this->timerid, 0, &(this->spec), NULL);
}

void tmr_startus_period(tmr_t* this, int us){
		this->spec.it_value.tv_sec = us;
	    this->spec.it_value.tv_nsec = 0;
	    this->spec.it_interval.tv_sec = us;
	    this->spec.it_interval.tv_nsec = 0;
	    timer_settime (this->timerid, 0, &(this->spec), NULL);
}

void tmr_startms(tmr_t* this, int ms) {
		this->spec.it_value.tv_sec = ms / 1000;
		this->spec.it_value.tv_nsec = (ms % 1000) * 1000000;
		this->spec.it_interval.tv_sec = 0;
		this->spec.it_interval.tv_nsec = 0;
		timer_settime (this->timerid, 0, &(this->spec), NULL);
}
/* tmr_stop_tmp
 * Para el timer pasado como parametro sin destruirlo
 * @param
 * 		tmr_t* this -> Timer que se quiere parar
 * @return
 * 		None
 *
 */
void tmr_stop_tmp(tmr_t* this){
	tmr_startms(this, 0);
}

void tmr_stop (tmr_t* this) {
    timer_delete(this->timerid);
}

//CallBack timers
/*
 * UpdateTimeState
 * Funcion de callback del timer que gestiona la duracion de las notas
 * Cambia flag_fsm poniendo a 1 FLAG_NOTA_TIMEOUT
 * @param
 * 		None
 * @return
 * 		None
 *
 */

void UpdateTimeState(){
	flag_fsm |= FLAG_NOTA_TIMEOUT;
	int tst = millis();
	printf("Fin Nota Millis: %d \n",tst);
}
