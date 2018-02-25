/*
 * main.h
 *
 *  Created on: 22 feb. 2018
 *      Author: jcala
 */

#ifndef SRC_FSM_CONTROLLER_H_
#define SRC_FSM_CONTROLLER_H_


#include <stdio.h>
#include <wiringPi.h>
#include <time.h>

#include "fsm.h"
#include "tipos.h"
#include "defines.h"
#include "timers.h"


typedef enum{
	WAIT_START = 0,
	WAIT_NEXT = 1,
	WAIT_END = 2
}_state;

typedef struct pibox_fsm{
		fsm_t fsm;
		TipoSistema* pibox;
}pibox_fsm_t;


timer_t* timerSound;

/*prototipos*/

fsm_t* getSoundFsm(int state, TipoSistema* user_data);


#endif /* SRC_FSM_CONTROLLER_H_ */
