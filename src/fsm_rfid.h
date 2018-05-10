/*
 * fsm_rfid.h
 *
 *  Created on: 8 may. 2018
 *      Author: Alejo
 */

#ifndef SRC_FSM_RFID_H_
#define SRC_FSM_RFID_H_

#include "fsm.h"
#include "defines.h"
#include "tipos.h"
#include "RC522.h"

uint8_t UUID[16];
typedef struct list_files{
	int num_files;
	int current_file;
	char** name_file;
}list_files_t;

enum flags_rfid{
	FLAG_SYSTEM_STARTn = 0x01,
	FLAG_CARD_IN = 0x02,
	FLAG_VALID_CARD =0x04,
	FLAG_SYSTEM_END =0x08,
	FLAG_CARD_EXIST = 0x10
};
enum rfid_states{
	WAIT_START,
	WAIT_PLAY ,
	WAIT_CARD ,
	WAIT_CHECK,
	WAIT_CONFIG,
	WAIT_DATA
};


fsm_t* fsm_rfid;
pthread_t thread;

int CompruebaComienzo(fsm_t* fsm);
int CompruebaFinalReproduccion(fsm_t* fsm);
int TarjetaDisponible(fsm_t* fsm);
int TarjetaNoDisponible(fsm_t* fsm);
int TarjetaValida(fsm_t* fsm);
int TarjetaNoValida(fsm_t* fsm);



void EsperaTargeta(fsm_t* fsm);
void DescartaTarjeta(fsm_t* fsm);
void LeerTarjeta(fsm_t* fsm);
void ComienzaSistema(fsm_t* fsm);
void CancelaReproduccion(fsm_t* fsm);
void ComienzaReproduccion(fsm_t* fsm);
void FinalizaReproduccion(fsm_t* fsm);
void CompruebaTarjeta(fsm_t* fsm);
void launchRFID();


#endif /* SRC_FSM_RFID_H_ */
