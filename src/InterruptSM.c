/*
 * InterruptSM.c
 *
 *  Created on: 8 may. 2018
 *      Author: Alejo
 */
#include "InterruptSM.h"
#include <stdio.h>
#include "bcm2835.h"
#include "poll.h"

#define THRESHOLD_HIGH 12 ///< Umbral para que el muestreo concluya nivel alto
#define THRESHOLD_LOW 7 ///< Umbral para que el muestreo concluya nivel bajo

/**
 * @brief Bucle de interrupción
 * 
 * @param userData 
 */
void loop(void *userData)
{
	ISR_Typ_ *ptr = (ISR_Typ_ *)userData;
	uint8_t pin = ptr->pin;
	int suma = 0;
	int contador = 0;
	int last_val = 0;
	while (1)
	{
		if (contador == 20)		//analizo en una ventana de 20 muestras
		{
			if (suma >= THRESHOLD_HIGH)			//si es un '1'
			{
				if (last_val == 0 && (ptr->event == FALLIN_EDGE || ptr->event == CHANGE))
				{
					ptr->callback(RISING_EDGE);
				}
				last_val = 1;
			}
			else if (suma <= THRESHOLD_LOW)			//si es un '0'
			{
				if (last_val == 1 && (ptr->event == RISING_EDGE || ptr->event == CHANGE))
				{
					ptr->callback(FALLIN_EDGE);		//llamo a la funcion del callback
				}
				last_val = 0;
			}
			suma = 0;
			contador = 0;
		}
		suma += bcm2835_gpio_lev(pin);
		contador++;
	}
}

/**
 * @brief Crea la interrupcion 
 * Crea un hilo donde se lanza una funcion que analizara el estado del pin para detectar cambios en el
 * @param PIN 	pin donde  se crea la interrupcion 
 * @param ISREvent Tipo de evento que queremos detectar
 * @param handdle 	(no used yet)
 * @param userData funcion de callback
 */
void attachIsr(uint8_t PIN, uint8_t ISREvent, void *handdle, void *userData)
{
	printf("Entro");
	fflush(stdout);
	if (!bcm2835_init())
		return;
	// Set RPI pin P1-15 to be an input
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
	//  with a pullup
	bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);
	// And a low detect enable
	pthread_t thread;
	ISR_Typ_ *interrupt = (ISR_Typ_ *)malloc(sizeof(ISR_Typ_));

	pthread_attr_t tattr;
	int newprio = 50;
	struct sched_param param;

	pthread_attr_init(&tattr);
	pthread_attr_getschedparam(&tattr, &param);
	param.sched_priority = newprio;
	pthread_attr_setschedparam(&tattr, &param);

	interrupt->callback = userData;
	interrupt->event = ISREvent;
	interrupt->pin = PIN;

	printf("Entro");
	fflush(stdout);
	if (threads[PIN] != 0)
	{
		printf("Error, has been created yet! \n");
		return;
	}
	pthread_create(&thread, &tattr, loop, interrupt);
	threads[PIN] = thread;
}
/**
 * @brief Elimina la interrupcion del pin especificado
 * 
 * @param PIN 
 */
void deleteIsr(uint8_t PIN)
{
	pthread_cancel(threads[PIN]);
	threads[PIN] = 0;
}
