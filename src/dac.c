/*
 * dac.c
 *
 *  Created on: 5 abr. 2018
 *      Author: jcala
 */

#include "dac.h"

#define set_addr(x) ((x)&0x0E)|0xC0

DAC* new_dac(uint8_t addr){
	DAC*  dac = (DAC*)malloc(sizeof(DAC));
	dac->buffer_pointer  = (int**)malloc(2*sizeof(int*));

	uint8_t ad = addr & 0xE;
	dac->addr = set_addr(addr);
	dac->currentBuff = 0;
	dac->pos = 0;

	*(dac->buffer_pointer) = dac->buffer0;
	*(dac->buffer_pointer+1) = dac->buffer1;
	return dac;
}

void reset_dac(DAC* dac){
	dac->currentBuff = 0;
	dac->pos = 0;
}


void flip_buffer(DAC* dac){

	dac->currentBuff = !dac->currentBuff;
	dac->pos = 0;

}
