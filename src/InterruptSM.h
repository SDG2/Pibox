/*
 * InterruptSM.h
 *
 *  Created on: 8 may. 2018
 *      Author: Alejo
 */

#ifndef INTERRUPTSM_H_
#define INTERRUPTSM_H_


#include <pthread.h>
#include <sched.h>
#include <stdint.h>
//macros
typedef void (*call_back) (int);

//variables
pthread_t threads[64]; ///<Hilos de las interrupciones
uint8_t atachPin;


//types 
typedef struct ISR_Typ{
	uint8_t pin;
	uint8_t event;
	call_back callback;
}ISR_Typ_;


enum event
{
	FALLIN_EDGE, ///< Flanco de bajada
	RISING_EDGE, ///< Flaco de subida
	LOW_DETECT,	///< Deteccion nivel alto
	HIGH_DETECT, ///< Deteccion nivel bajo
	CHANGE ///< Cambio de nivel
};

//Prototypes

void attachIsr(uint8_t PIN, uint8_t ISREvent, void* handdle, void* userData );
void deleteIsr(uint8_t PIN);

#endif /* INTERRUPTSM_H_ */
