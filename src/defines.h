/*
 * defines.h
 *
 *  Created on: 25 feb. 2018
 *      Author: Alejo
 */

#ifndef SRC_DEFINES_H_
#define SRC_DEFINES_H_


/*
 * Defines globales que afectan al funcionamiento del programa
 * o activan y/o modifican funcionalidades del mismo
 *
 * */
//Necesario para funcionar con BCM
#define bcm_spi


#define CANCION_ACABADA 1
#define CANCION_NOACABADA 0;

#define use_bcm

#ifdef use_wiringPI
	#define PIN_PWM 12
#else
	#define PIN_PWM 18
#endif





#endif /* SRC_DEFINES_H_ */
