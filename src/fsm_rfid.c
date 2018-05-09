/*
 * fsm_rfid.c
 *
 *  Created on: 8 may. 2018
 *      Author: Alejo
 */


#include "fsm_rfid.h"
#include "mutex.h"
#include <stdio.h>

fsm_trans_t transition_table_rfid[] = {
		{WAIT_START, CompruebaComienzo,WAIT_CARD,ComienzaSistema},
		{WAIT_CARD,TarjetaDisponible,WAIT_CHECK, LeerTarjeta},
		{WAIT_CARD,TarjetaNoDisponible,WAIT_CARD,EsperaTargeta},
		{WAIT_CHECK,TarjetaNoValida,WAIT_CARD,DescartaTarjeta},
		{WAIT_CHECK,TarjetaValida,WAIT_PLAY,ComienzaReproduccion},
		{WAIT_PLAY,TarjetaDisponible,WAIT_PLAY,CompruebaTarjeta},
		{WAIT_PLAY,TarjetaNoDisponible,WAIT_START,CancelaReproduccion},
		{WAIT_PLAY,CompruebaFinalReproduccion,WAIT_START,FinalizaReproduccion},
		{-1, NULL, -1, NULL }
};

int CompruebaComienzo(fsm_t* fsm){
	return 1;
}

int CompruebaFinalReproduccion(fsm_t* fsm){
	return flag_rfid & FLAG_SYSTEM_END;
}

int TarjetaDisponible(fsm_t* fsm){
	return flag_rfid & FLAG_CARD_IN;
	//return 1;
}
int TarjetaNoDisponible(fsm_t* fsm){
	return !(flag_rfid & FLAG_CARD_IN);
	//return 0;
}
int TarjetaValida(fsm_t* fsm){
	printf("Flag: %d \n", flag_rfid);
	return flag_rfid & FLAG_VALID_CARD;
}
int TarjetaNoValida(fsm_t* fsm){
	return !(flag_rfid & FLAG_VALID_CARD);
}

void EsperaTargeta(fsm_t* fsm){
	return;
}
void DescartaTarjeta(fsm_t* fsm){
	flag_rfid &= ~FLAG_CARD_IN;
}
void LeerTarjeta(fsm_t* fsm){
	printf("LEE \n");
	if(RC522_Check(UUID) == STATUS_OK){
		printf("FOUND TAG! \n");
		flag_rfid |= FLAG_VALID_CARD;
	}
}
void ComienzaReproduccion(fsm_t* fsm){
	printf("eNTROOOO");
	fflush(stdout);
	lock(0);
	flags_player |= FLAG_START;
	unlock(0);
	printf("comienzas flag: %d", flags_player);

}
void CancelaReproduccion(fsm_t* fsm){
	lock(0);
	flags_player |= FLAG_END;
	unlock(0);
}
void CompruebaTarjeta(fsm_t* fsm){
	//interrup
}
void FinalizaReproduccion(fsm_t* fsm){
	lock(0);
	flags_player |= FLAG_END;
	unlock(0);
}
void ComienzaSistema(fsm_t* fsm){
	if(RC522_Init() == STATUS_ERROR)
		printf("ERROR! \n");
	flag_rfid = 0;
}

void lp(void* userData){
	fsm_t* data = (fsm_t*)userData;
	while(1){
		fsm_fire(data);
	}
}
void launchRFID(){
	fsm_rfid = fsm_new(transition_table_rfid,NULL);
	pthread_create(&thread,NULL, lp, fsm_rfid);
}

void killRFID(){

}




