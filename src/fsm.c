/*
 * fsm.c
 *
 *  Created on: 22 feb. 2018
 *      Author: jcala
 */
#include "fsm.h"


fsm_t* fsm_new (fsm_trans_t* tt){
	fsm_t* new_fsm = (fsm_t*) malloc (sizeof (fsm_t));
	new_fsm->tt = tt;
	new_fsm->current_state = 0;
	return new_fsm;
}

int fsm_delete(fsm_t* fsm){
	free(fsm);
	return 1;
}

void fsm_fire(fsm_t* this) {
	fsm_trans_t* t;
	for (t = this->tt; t->orig_state >= 0; ++t) {
		if ((this->current_state == t->orig_state) && t->in(this)) {
			this->current_state = t->dest_state;
			if (t->out)
				t->out(this);
			break;
		}
	}
}
