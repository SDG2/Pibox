
#include "fsm_controller.h"
#include "tmr.h"

//#define DEBUG
/* private testing functions prototypes*/

static int CompruebaPlayerStart(fsm_t* fsm);
static int Comprueba_nota_timeout(fsm_t* fsm);
static int CompruebaNuevaNota(fsm_t* fsm);
static int CompruebaPlayerStop(fsm_t* fsm);
static int CompruebaFinalMelodia(fsm_t* fsm);

/*private output functions prototypes*/

static void Iniciliza_player(fsm_t* fsm);
static void Actualiza_player(fsm_t* fsm);
static void Comienza_nueva_nota(fsm_t* fsm);
static void Stop_Player(fsm_t* fsm);
static void Final_Melodia(fsm_t* fsm);

/* Another private prototype*/


/* transition_table*/


fsm_trans_t transition_table[] = {
		{WAIT_START, CompruebaPlayerStart,WAIT_NEXT,Iniciliza_player},
		{WAIT_NEXT,Comprueba_nota_timeout,WAIT_END,Actualiza_player},
		{WAIT_NEXT,CompruebaPlayerStop,WAIT_START,Stop_Player},
		{WAIT_END,CompruebaFinalMelodia,WAIT_START,Final_Melodia},
		{WAIT_END,CompruebaNuevaNota,WAIT_NEXT,Comienza_nueva_nota},
		{-1, NULL, -1, NULL }
};

/*
 * Compruebaxxxxxx
 * @param
 * 		fsm_t* fsm -> fsm context
 * @return
 * 		'1' if it is active
 *
 */
static int CompruebaPlayerStart(fsm_t* fsm){
	if(flag_fsm & FLAG_PLAYER_START){
		return 1;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_START a WAIT_NEXT");
	#endif
	return 0;
}

static int Comprueba_nota_timeout(fsm_t* fsm){

	if(flag_fsm & FLAG_NOTA_TIMEOUT){
		printf("LEO FLAG \n");
		fflush(stdout);
		return 1;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_NEXT a WAIT_END");
	#endif
	return 0;
}


static int CompruebaNuevaNota(fsm_t* fsm){
	if(flag_fsm & FLAG_PLAYER_END){
		return 1;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_END a WAIT_NEXT");
	#endif
	return 0;
}

static int CompruebaPlayerStop(fsm_t* fsm){
	if((flag_fsm & FLAG_PLAYER_STOP)){
		return 1;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_NEXT a WAIT_START");
	#endif
	return 0;
}

static int CompruebaFinalMelodia(fsm_t* fsm){
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	if( (flag_fsm & FLAG_PLAYER_STOP) && (pi_box_fsm->pibox->player.posicion_nota_actual ==	pi_box_fsm->pibox->player.melodia->num_notas -1) )
		return 1;
	#ifdef DEBUG
		printf("Cambio de WAIT_END a WAIT_START");
	#endif
	return 0;
}





/*
 *
 */

static void Iniciliza_player(fsm_t* fsm){
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	pi_box_fsm->pibox =(TipoSistema*)fsm->user_data;
	printf("Inicio \n");
	pi_box_fsm->pibox->player.posicion_nota_actual 	= 0;
	pi_box_fsm->pibox->player.frecuencia_nota_actual = pi_box_fsm->pibox->player.melodia->frecuencias[0];
	pi_box_fsm->pibox->player.duracion_nota_actual 	= pi_box_fsm->pibox->player.melodia->duraciones[0];

	//Inicio el Timer
	tmr_startms(pi_box_fsm->pibox->timerSound,pi_box_fsm->pibox->player.duracion_nota_actual);
	//Quito Flag de Stop si hubiese
	if(flag_fsm & FLAG_PLAYER_STOP)
		flag_fsm ^= FLAG_PLAYER_STOP;
	#ifdef DEBUG
		printf("Inicia player");
	#endif

}

static void Actualiza_player(fsm_t* fsm){
	#ifdef DEBUG
		printf("Actualiza player");
	#endif

	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	pi_box_fsm->pibox =(TipoSistema*)pi_box_fsm->fsm.user_data;
	pi_box_fsm->pibox->player.posicion_nota_actual 	= (pi_box_fsm->pibox->player.posicion_nota_actual)+1;
	pi_box_fsm->pibox->player.frecuencia_nota_actual 	= pi_box_fsm->pibox->player.melodia->frecuencias[pi_box_fsm->pibox->player.posicion_nota_actual];
	pi_box_fsm->pibox->player.duracion_nota_actual 	= pi_box_fsm->pibox->player.melodia->duraciones[pi_box_fsm->pibox->player.posicion_nota_actual] ;
	printf("Actualizo %d \n",(pi_box_fsm->pibox->player.posicion_nota_actual));
	tmr_stop_tmp(pi_box_fsm->pibox->timerSound);
	flag_fsm = FLAG_PLAYER_END;

}
static void Comienza_nueva_nota(fsm_t* fsm){
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	#ifdef DEBUG
		printf("COMIENZA %d \n",(pi_box_fsm->pibox->player.posicion_nota_actual));
	#endif

	//softToneWrite(PIN_PWM , pi_box_fsm->pibox->player.frecuencia_nota_actual);
	tmr_startms(pi_box_fsm->pibox->timerSound,pi_box_fsm->pibox->player.duracion_nota_actual);
}


static void Stop_Player(fsm_t* fsm){
	//softToneWrite(PIN_PWM,0);
	printf("STOP PLAYER \n");
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	tmr_stop_tmp(pi_box_fsm->pibox->timerSound);
}
static void Final_Melodia(fsm_t* fsm){
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	tmr_stop_tmp(pi_box_fsm->pibox->timerSound);
	flag_fsm ^= FLAG_PLAYER_START;
}










