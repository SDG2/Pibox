//
//#include "fsm_controller.h"
//#include "tmr.h"
//#include "tone.h"
//#include "defines.h"
//#include <softTone.h>
//#include <wiringPi.h>
//
//
////#define DEBUG
///* private testing functions prototypes*/
//
//static int CompruebaPlayerStart(fsm_t* fsm);
//static int Comprueba_nota_timeout(fsm_t* fsm);
//static int CompruebaNuevaNota(fsm_t* fsm);
//static int CompruebaPlayerStop(fsm_t* fsm);
//static int CompruebaFinalMelodia(fsm_t* fsm);
//
///*private output functions prototypes*/
//
//static void Iniciliza_player(fsm_t* fsm);
//static void Actualiza_player(fsm_t* fsm);
//static void Comienza_nueva_nota(fsm_t* fsm);
//static void Stop_Player(fsm_t* fsm);
//static void Final_Melodia(fsm_t* fsm);
//
///* Another private prototype*/
///*
// * Funciones de arranque y parada del PWM
// *
// * */
//extern void UpdateTimeState();
//void parar_melodia();
//void inicia_nota(uint32_t freq);
//
///* transition_table*/
//fsm_trans_t transition_table[] = {
//		{WAIT_START, CompruebaPlayerStart,WAIT_NEXT,Iniciliza_player},
//		{WAIT_NEXT,Comprueba_nota_timeout,WAIT_END,Actualiza_player},
//		{WAIT_NEXT,CompruebaPlayerStop,WAIT_START,Stop_Player},
//		{WAIT_END,CompruebaFinalMelodia,WAIT_START,Final_Melodia},
//		{WAIT_END,CompruebaNuevaNota,WAIT_NEXT,Comienza_nueva_nota},
//		{-1, NULL, -1, NULL }
//};
//
///*
// * CompruebaXXXXXX
// * @param
// * 		fsm_t* fsm -> fsm context
// * @return
// * 		'1' if it is active
// *
// */
//static int CompruebaPlayerStart(fsm_t* fsm){
//	if(flag_fsm & FLAG_PLAYER_START){
//		return 1;
//	}
//	#ifdef DEBUG
//		printf("Cambio de WAIT_START a WAIT_NEXT");
//	#endif
//	return 0;
//}
//
//static int Comprueba_nota_timeout(fsm_t* fsm){
//
//	if(flag_fsm & FLAG_NOTA_TIMEOUT){
//		printf("LEO FLAG \n");
//		fflush(stdout);
//		return 1;
//	}
//	#ifdef DEBUG
//		printf("Cambio de WAIT_NEXT a WAIT_END");
//	#endif
//	return 0;
//}
//
//
//static int CompruebaNuevaNota(fsm_t* fsm){
//	if(!(flag_fsm & FLAG_PLAYER_END)){
//		return 1;
//	}
//	#ifdef DEBUG
//		printf("Cambio de WAIT_END a WAIT_NEXT");
//	#endif
//	return 0;
//}
//
//static int CompruebaPlayerStop(fsm_t* fsm){
//	if((flag_fsm & FLAG_PLAYER_STOP)){
//		return 1;
//	}
//	#ifdef DEBUG
//		printf("Cambio de WAIT_NEXT a WAIT_START");
//	#endif
//	return 0;
//}
//
//static int CompruebaFinalMelodia(fsm_t* fsm){
//
//	if((flag_fsm & FLAG_PLAYER_END))
//		return 1;
//	#ifdef DEBUG
//		printf("Cambio de WAIT_END a WAIT_START");
//	#endif
//	return 0;
//}
//
//
//
//
//
///*
// * Iniciliza_player
// * Inicia la maquina de estados tras haber sido registrado un evento de
// * flag_player_start. Carga la primera nota en tipo player e inicia su reproduccion con su
// * timer asociado. Resetea todos los flags del sistema
// * @param
// * 		fsm_t* fsm -> Puntero al tipo de la maquina de estados
// * @return
// * 		None
// *
// */
//
//static void Iniciliza_player(fsm_t* fsm){
//	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
//	pi_box_fsm->pibox =(TipoSistema*)fsm->user_data;
//	printf("Inicio \n");
//	pi_box_fsm->pibox->player.posicion_nota_actual 	= 0;
//	pi_box_fsm->pibox->player.frecuencia_nota_actual = pi_box_fsm->pibox->player.melodia->frecuencias[0];
//	pi_box_fsm->pibox->player.duracion_nota_actual 	= pi_box_fsm->pibox->player.melodia->duraciones[0];
//	int tst = millis();
//	printf("Inicio Nota Millis: %d \n",tst);
//	//Hago que suene la nota
//	inicia_nota(pi_box_fsm->pibox->player.frecuencia_nota_actual);
//	//Inicio el Timer
//	tmr_startms(pi_box_fsm->pibox->timerSound,pi_box_fsm->pibox->player.duracion_nota_actual);
//	flag_fsm = 0;
//	//Quito Flag de Stop si hubiese
//	#ifdef DEBUG
//		printf("Inicia player");
//	#endif
//}
///*
// * Actualiza_player
// * Si no ha acabado la canción carga la nota siguiente en
// * tipo player, si ha acabado, pone FLAG_PLAYER_END a 1 en flag_fsm.
// * Resetea siempre FLAG_NOTA_TIMEOUT
// * @param
// * 		fsm_t* fsm -> Puntero al tipo de la maquina de estados
// * @return
// * 		None
// *
// */
//static void Actualiza_player(fsm_t* fsm){
//	#ifdef DEBUG
//		printf("Actualiza player");
//	#endif
//	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
//	pi_box_fsm->pibox =(TipoSistema*)pi_box_fsm->fsm.user_data;
//	int tst = millis();
//	printf("Inicio Nota Millis: %d \n",tst);
//	tmr_stop_tmp(pi_box_fsm->pibox->timerSound);
//	flag_fsm &= ~FLAG_NOTA_TIMEOUT;
//	//delay
//	//delay(1000);
//	if(pi_box_fsm->pibox->player.posicion_nota_actual != pi_box_fsm->pibox->player.melodia->num_notas - 1){
//		pi_box_fsm->pibox->player.posicion_nota_actual 	= (pi_box_fsm->pibox->player.posicion_nota_actual)+1;
//		pi_box_fsm->pibox->player.frecuencia_nota_actual = pi_box_fsm->pibox->player.melodia->frecuencias[pi_box_fsm->pibox->player.posicion_nota_actual];
//		pi_box_fsm->pibox->player.duracion_nota_actual 	= pi_box_fsm->pibox->player.melodia->duraciones[pi_box_fsm->pibox->player.posicion_nota_actual];
//		printf("Actualizo %d \n",(pi_box_fsm->pibox->player.posicion_nota_actual));
//	}else{
//		flag_fsm |= FLAG_PLAYER_END;
//	}
//
//}
///*
// * Comienza_nueva_nota
// * Inicia la nota cargada en el tipo player en el PWM y
// * carga el timer con el valor correspondiente a la nota
// * @param
// * 		fsm_t* fsm -> Puntero al tipo de la maquina de estados
// * @return
// * 		None
// *
// */
//
//static void Comienza_nueva_nota(fsm_t* fsm){
//	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
//	#ifdef DEBUG
//		printf("COMIENZA %d \n",(pi_box_fsm->pibox->player.posicion_nota_actual));
//	#endif
//	inicia_nota(pi_box_fsm->pibox->player.frecuencia_nota_actual);
//	printf("Actualizo %d \n",(pi_box_fsm->pibox->player.posicion_nota_actual));
//	printf("Frecuencia %d \n Tiempo %d \n",pi_box_fsm->pibox->player.frecuencia_nota_actual,pi_box_fsm->pibox->player.duracion_nota_actual);
//	tmr_startms(pi_box_fsm->pibox->timerSound,pi_box_fsm->pibox->player.duracion_nota_actual);
//}
///*
// * Stop_Player
// * Para la melodía, desarma el timer que controla los tiempos de las notas
// *
// * @param
// * 		fsm_t* fsm -> Puntero al tipo de la maquina de estados
// * @return
// * 		None
// *
// */
//static void Stop_Player(fsm_t* fsm) {
//	//softToneWrite(PIN_PWM,0);
//	printf("STOP PLAYER \n");
//#ifdef DEBUG
//	printf("%d",flag_fsm);
//#endif
//	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*) fsm;
//	parar_melodia();
//	tmr_stop_tmp(pi_box_fsm->pibox->timerSound);
//}
//
///*
// * Final_Melodia
// * Para la melodía, desarma el timer que controla los tiempos de las notas
// *
// * @param
// * 		fsm_t* fsm -> Puntero al tipo de la maquina de estados
// * @return
// * 		None
// *
// */
//
//static void Final_Melodia(fsm_t* fsm){
//	pibox_fsm_t* pi_box_fsm = (pibox_fsm_t*)fsm;
//	parar_melodia();
//	tmr_stop_tmp(pi_box_fsm->pibox->timerSound);
//	flag_fsm &= ~FLAG_PLAYER_START;
//}
//
///*
// * inicia_nota
// * @param
// * 		uint32_t freq -> frecuencia de la nota que queremos reproducir
// * 		Usa una libreria u otra en funcion del define
// * @return
// * 		None
// *
// */
//void inicia_nota(uint32_t freq){
//#ifdef use_wiringPI
//	softToneWrite(PIN_PWM , freq);
//#endif
//#ifdef use_bcm
//	tone_write(freq);
//#endif
//}
///*
// * parar_melodia
// * Para la melodía
// * Usa una libreria u otra en funcion del define
// * @param
// * 	None
// * @return
// * 		None
// *
// */
//void parar_melodia(){
//#ifdef use_wiringPI
//	softToneWrite(PIN_PWM ,0);
//#endif
//#ifdef use_bcm
//	tone_write(0);
//#endif
//}
//
//
//
//
//
//
//
//
//
