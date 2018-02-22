/*
 * fsm.h
 *
 *  Created on: 22 feb. 2018
 *      Author: jcala
 */

#ifndef SRC_FSM_H_
#define SRC_FSM_H_

#include <stdlib.h>

typedef struct fsm_ fsm_t;

typedef int (*fsm_input_func_t)(fsm_t*);
typedef void (*fsm_output_func_t)(fsm_t*);

typedef struct fsm_trans_ {
	int orig_state;
	fsm_input_func_t in;
	int dest_state;
	fsm_output_func_t out;
} fsm_trans_t;

struct fsm_ {
	int current_state;
	fsm_trans_t* tt;
	void* user_data;
};

fsm_t* fsm_new (fsm_trans_t* tt);
void fsm_fire (fsm_t* fsm);
int fsm_delete(fsm_t* fsm);

#endif /* SRC_FSM_H_ */
