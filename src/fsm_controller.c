
#include "fsm_controller.h"

static int CompruebaPlayerStart(fsm_t* fsm);
static int Comprueba_nota_timeout(fsm_t* fsm);
static int CompruebaNuevaNota(fsm_t* fsm);
static int CompruebaPlayerStop(fsm_t* fsm);
static int CompruebaFinalMelodia(fsm_t* fsm);

static void Iniciliza_player(fsm_t* fsm);
static void Actualiza_player(fsm_t* fsm);
static void Comienza_nueva_nota(fsm_t* fsm);
static void Stop_Player(fsm_t* fsm);
static void Final_Melodia(fsm_t* fsm);

fsm_trans_t nevera_transition_table[] = {
		{WAIT_START, CompruebaPlayerStart,WAIT_NEXT,Iniciliza_player},
		{WAIT_NEXT,Comprueba_nota_timeout,WAIT_END,Actualiza_player},
		{WAIT_NEXT,CompruebaPlayerStop,WAIT_START,Stop_Player},
		{WAIT_END,CompruebaNuevaNota,WAIT_NEXT,Comienza_nueva_nota},
		{WAIT_END,CompruebaFinalMelodia,WAIT_START,Final_Melodia},
		{-1, NULL, -1, NULL },
};


static int CompruebaPlayerStart(fsm_t* fsm){
	pibox_fsm_t* pibox = (pibox_fsm_t*)fsm;
	if(flags_fsm & 0x01){
		return 1;
	}
	return 0;
}
static int Comprueba_nota_timeout(fsm_t* fsm){
	pibox_fsm_t* pibox = (pibox_fsm_t*)fsm;
	if(flags_fsm & 0x08){
		return 1;
	}
	return 0;
}
static int CompruebaNuevaNota(fsm_t* fsm){
	pibox_fsm_t* pibox = (pibox_fsm_t*)fsm;
	if(flags_fsm & 0x10){
		return 1;
	}
	return 0;
}
static int CompruebaPlayerStop(fsm_t* fsm){
	pibox_fsm_t* pibox = (pibox_fsm_t*)fsm;
	if((flags_fsm & 0x02)){
		return 1;
	}
	return 0;
}
static int CompruebaFinalMelodia(fsm_t* fsm){
	pibox_fsm_t* pibox = (pibox_fsm_t*)fsm;
	if(flags_fsm & 0x04){
		return 1;
	}
	return 0;
}

static void Iniciliza_player(fsm_t* fsm){
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	pi_box_fsm->pibox =(TipoSistema*)pi_box_fsm->fsm->user_data;
}
static void Actualiza_player(fsm_t* fsm){

}
static void Comienza_nueva_nota(fsm_t* fsm){

}
static void Stop_Player(fsm_t* fsm){

}
static void Final_Melodia(fsm_t* fsm){

}
