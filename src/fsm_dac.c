/*
 * fsm_dac.c
 *
 *  Created on: 5 abr. 2018
 *      Author: jcala
 */


#include "fsm_dac.h"
#include "fsm.h"
#include "tipos.h"
#include <stdint.h>

#define S_BYTE(x) (x)&0x0FFF
#define L_BYTE(x) (uint8_t)((x)&0x000F)


union uint16_2_uint8 {
	uint8_t  byte[2];
    uint16_t word;
};

static int CompruebaStart(fsm_t* fsm);
static int CompruebaStop(fsm_t* fsm);
static int CompruebaMitad(fsm_t* fsm);
void SendData(fsm_t* fsm);
void InitDac(fsm_t* fsm);
void CleanData(fsm_t* fsm);

fsm_trans_t transition_table[] = {
		{PARADO,CompruebaStart,MANDA,InitDac},
		{MANDA,CompruebaStop,PARADO,CleanData},
		{MANDA ,CompruebaMitad,MANDA,SendData},
		{-1, NULL, -1, NULL }
};

static int CompruebaStart(fsm_t* fsm){
	return flag_dac & FLAG_DAC_START;
}
static int CompruebaStop(fsm_t* fsm){
	return flag_dac & FLAG_DAC_STOP;
}
static int CompruebaMitad(fsm_t* fsm){
	dac_fsm_t* dac_fsm = (dac_fsm_t*)fsm;
	if(dac_fsm->dac->pos <= BUFF_LENGTH/2){
		//TODO: HACER MUTEX
		if(dac_fsm->dac->currentBuff == 0){
			flag_dac |= FLAG_DAC_HALF_0;
		}else if(dac_fsm->dac->currentBuff == 1){
			flag_dac |= FLAG_DAC_HALF_1;
		}
		return 1;
	}
	return 0;
}

void SendData(fsm_t* fsm){
	union uint16_2_uint8 data_converter;

	dac_fsm_t* dac_fsm = (dac_fsm_t*)fsm;

	data_converter.word = S_BYTE(dac_fsm->dac->buffer_pointer[dac_fsm->dac->currentBuff][dac_fsm->dac->pos]);
	bcm2835_i2c_setSlaveAddress(dac_fsm->dac->addr);
	bcm2835_i2c_write(data_converter.byte,2);
	bcm2835_i2c_end();
	if(++dac_fsm->dac->pos == BUFF_LENGTH){
		flip_buffer(dac_fsm->dac);
	}
}

void InitDac(fsm_t* fsm){
	dac_fsm_t* dac_fsm = (dac_fsm_t*)fsm;
	if(!bcm2835_i2c_begin()){
		#ifdef DEBUG
			printf("I2C init error. are u root?");
		#endif
		return;
	}
	dac_fsm->dac = (DAC*)fsm->user_data;
}

void CleanData(fsm_t* fsm){
	dac_fsm_t* dac_fsm = (dac_fsm_t*)fsm;
	reset_dac(dac_fsm->dac);
}

