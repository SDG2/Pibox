/*
 * tipos.h
 *
 *  Created on: 22 feb. 2018
 *      Author: jcala
 */


#ifndef SRC_TIPOS_H_
#define SRC_TIPOS_H_

#include <stdint.h>
#include "tmr.h"

#define MAX_NUM_TARJETAS	5
#define MAX_NUM_NOTAS 		600
#define MAX_NUM_CHAR_NOMBRE	100
#define MAX_NUM_CHAR_UID	100


//Estructuras
typedef struct {
	char* nombre; // String con el nombre de la melodia
	int* frecuencias; // Array con las frecuencias de las notas de la melodia
	int* duraciones; // Array con las duraciones de las notas de la melodia
	int num_notas; // Numero de notas de que consta la melodia
} TipoMelodia;

typedef struct {
	int posicion_nota_actual;	// Valor correspondiente a la posicion de la nota actual en los arrays de frecuencias y duraciones
	int frecuencia_nota_actual; // Valor correspondiente a la frecuencia de la nota actual
	int duracion_nota_actual; // Valor correspondiente a la duracion de la nota actual

	TipoMelodia* melodia;

	// A completar por el alumno
	// ...
} TipoPlayer;

typedef struct {
	//Uid uid;  // Identificador correspondiente a la tarjeta (tipo de datos complejo definido en liberia control RFID)
	TipoMelodia melodia;
} TipoTarjeta;

typedef struct {
	TipoPlayer player; // Reproductor de melodias

	int num_tarjetas_activas; // Numero de tarjetas validas
	TipoTarjeta tarjetas_activas[MAX_NUM_TARJETAS]; // Array con todas las tarjetas validas
	//Uid uid_tarjeta_actual; // Identificador de la tarjeta actual (tipo de datos complejo definido en liberia control RFID)
	int pos_tarjeta_actual; // Posicion de la tarjeta actual en el array de tarjetas validas
	char uid_tarjeta_actual_string[MAX_NUM_CHAR_UID]; // Identificador de la tarjeta actual a modo de string de caracteres
	char teclaPulsada; // Variable que almacena la ultima tecla pulsada
	int debug; // Variable que habilita o deshabilita la impresion de mensajes por salida estandar
	tmr_t* timerSound;
} TipoSistema;



//Vatiables globales
/*
 * 0 ->	FLAG_PLAYER_START
 * 1 -> FLAG_PLATER_STOP
 * 2 ->	FLAG_PLAYER_END
 * 3 -> FLAG_NOTA_TIMEOUT
 * 4 -> FLAG_NOTA_END
 * */

#define	FLAG_PLAYER_START 0x01
#define	FLAG_PLAYER_STOP  0x02
#define	FLAG_PLAYER_END   0x04
#define	FLAG_NOTA_TIMEOUT 0x08
#define	FLAG_QUIT	      0x10


#define FLAG_BFF1_END 0x01
#define FLAG_BFF2_END 0x02
#define FLAG_END 0x04
#define FLAG_START 0x08

#define FLAG_IRQ_STEPPER_CONTINUE 0x01
#define	FLAG_IRQ_STEPPER_SELECT	0x02

volatile uint8_t flag_fsm;
volatile  uint8_t flags_player ;
volatile  uint8_t flag_rfid;
volatile uint8_t stepper_irq_flag;
volatile char* song_name;
volatile int num_file;
#endif /* SRC_TIPOS_H_ */
