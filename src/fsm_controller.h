/*
 * fsm_controller.h
 *
 *  Created on: 1 mar. 2018
 *      Author: Alejo
 */

#ifndef SRC_FSM_CONTROLLER_H_
#define SRC_FSM_CONTROLLER_H_

#include <stdio.h>
#include <wiringPi.h>
#include <softTone.h>
#include <time.h>


#include "fsm.h"
#include "defines.h"
#include "tipos.h"
#include "bcm2835.h"



typedef enum{
	WAIT_START = 0,
	WAIT_NEXT = 1,
	WAIT_END = 2
}_state;

/*
 * Tipo que hereda del propio de la maquina de estados,
 * para una mejor gestion de la maquina de estados
 * */
typedef struct pibox_fsm{
		fsm_t fsm;
		TipoSistema* pibox;
}pibox_fsm_t;



/*prototipos*/

//fsm_t* getSoundFsm(int state, TipoSistema* user_data);

#endif /* SRC_FSM_CONTROLLER_H_ */
