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

#include "fsm.h"
#include "tipos.h"


typedef enum{
	WAIT_START = 0,
	WAIT_NEXT = 1,
	WAIT_END = 2
}_state;

typedef struct pibox_fsm{
		fsm_t fsm;
		TipoSistema* pibox;
}pibox_fsm_t;

//nevera_fsm_t* new_nevera_fsm(fsm_trans_t* nevera_transition_table);
//int delete_nevera_fsm(nevera_fsm_t* nevera_fsm);





#endif /* SRC_FSM_CONTROLLER_H_ */
