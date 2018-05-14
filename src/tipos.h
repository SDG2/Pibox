/*
 * tipos.h
 *
 *  Created on: 22 feb. 2018
 *      Author: jcala
 */


#ifndef SRC_TIPOS_H_
#define SRC_TIPOS_H_

#include <stdint.h>

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
#define FLAG_IRQ_STEPPER_DIR 0x04 //0-> izq 1-> der

volatile uint8_t flag_fsm;
volatile  uint8_t flags_player ;
volatile  uint8_t flag_rfid;
volatile uint8_t stepper_irq_flag;
volatile char* song_name;
volatile int num_file;


typedef struct {

} TipoSistema;
#endif /* SRC_TIPOS_H_ */
