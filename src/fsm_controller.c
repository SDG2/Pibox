
#include "fsm_controller.h"


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

void UpdateTimeState();

/* transition_table*/
timer_t timerSound;
timer_t timerSound2;
fsm_trans_t transition_table[] = {
		{WAIT_START, CompruebaPlayerStart,WAIT_NEXT,Iniciliza_player},
		{WAIT_NEXT,Comprueba_nota_timeout,WAIT_END,Actualiza_player},
		{WAIT_NEXT,CompruebaPlayerStop,WAIT_START,Stop_Player},
		{WAIT_END,CompruebaNuevaNota,WAIT_NEXT,Comienza_nueva_nota},
		{WAIT_END,CompruebaFinalMelodia,WAIT_START,Final_Melodia},
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
	if(flag_fsm &~ FLAG_PLAYER_START){
		return 0;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_START a WAIT_NEXT");
	#endif
	return 1;
}

static int Comprueba_nota_timeout(fsm_t* fsm){
	if(flag_fsm &~ FLAG_NOTA_TIMEOUT){
		return 0;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_NEXT a WAIT_END");
	#endif
	return 1;
}


static int CompruebaNuevaNota(fsm_t* fsm){
	if(flag_fsm &~ FLAG_PLAYER_END){
		return 1;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_END a WAIT_NEXT");
	#endif
	return 0;
}

static int CompruebaPlayerStop(fsm_t* fsm){
	if((flag_fsm &~ FLAG_PLATER_STOP)){
		return 0;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_NEXT a WAIT_START");
	#endif
	return 1;
}

static int CompruebaFinalMelodia(fsm_t* fsm){
	if(flag_fsm &~ FLAG_PLAYER_END){
		return 0;
	}
	#ifdef DEBUG
		printf("Cambio de WAIT_END a WAIT_START");
	#endif
	return 1;
}





/*
 *
 */

static void Iniciliza_player(fsm_t* fsm){
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	pi_box_fsm->pibox =(TipoSistema*)fsm->user_data;
	struct sigevent se;

	pi_box_fsm->pibox->player.posicion_nota_actual 	= 0;
	pi_box_fsm->pibox->player.frecuencia_nota_actual 	= pi_box_fsm->pibox->player.melodia->frecuencias[0];
	pi_box_fsm->pibox->player.duracion_nota_actual 	= pi_box_fsm->pibox->player.melodia->duraciones[0];

	#ifdef DEBUG
		printf("Inicia player");
	#endif

		se.sigev_notify = SIGEV_THREAD;
		se.sigev_value.sival_ptr = &timerSound;
		se.sigev_notify_function = UpdateTimeState;
		se.sigev_notify_attributes = NULL;
		if(timer_create(CLOCK_MONOTONIC, &se, &timerSound)	==	-1)
			printf("funciono");
		timerIdInit(&timerSound2,UpdateTimeState);
}

static void Actualiza_player(fsm_t* fsm){
	#ifdef DEBUG
		printf("Actualiza player");
	#endif
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	pi_box_fsm->pibox =(TipoSistema*)pi_box_fsm->fsm.user_data;
	pi_box_fsm->pibox->player.posicion_nota_actual 	= (pi_box_fsm->pibox->player.posicion_nota_actual)+1;
	pi_box_fsm->pibox->player.frecuencia_nota_actual 	= pi_box_fsm->pibox->player.melodia->frecuencias[ pi_box_fsm->pibox->player.posicion_nota_actual ];
	pi_box_fsm->pibox->player.duracion_nota_actual 	= pi_box_fsm->pibox->player.melodia->duraciones[ pi_box_fsm->pibox->player.posicion_nota_actual] ;

	flag_fsm = 0xFF;

	if(pi_box_fsm->pibox->player.posicion_nota_actual	==	pi_box_fsm->pibox->player.melodia->num_notas -1 )
		flag_fsm = FLAG_PLAYER_END;

}
static void Comienza_nueva_nota(fsm_t* fsm){
	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
	pi_box_fsm->pibox =(TipoSistema*)pi_box_fsm->fsm.user_data;
	#ifdef DEBUG
		printf("Comienza nota");
	#endif
	softToneWrite(PIN_PWM , pi_box_fsm->pibox->player.frecuencia_nota_actual);
	timerIdStart(timerSound, pi_box_fsm->pibox->player.duracion_nota_actual);
}


static void Stop_Player(fsm_t* fsm){
	softToneWrite(PIN_PWM,0);
	timerIdStop(timerSound);
}
static void Final_Melodia(fsm_t* fsm){
	timerIdStop(timerSound);
}




void UpdateTimeState(){
	flag_fsm = FLAG_NOTA_TIMEOUT;
	timerIdStop(timerSound);
}

//

/*fsm_t* getSoundFsm(int state, TipoSistema* user_data){
	fsm_t* fsm = fsm_new(transition_table);
	fsm->current_state = state;
	fsm->user_data = user_data;
	return fsm;
}*/




