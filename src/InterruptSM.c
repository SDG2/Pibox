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

#define THRESHOLD_HIGH 30
#define THRESHOLD_LOW  20



void loop(void* userData){
	ISR_Typ_* ptr = (ISR_Typ_*)userData;
	uint8_t pin = ptr->pin;
	int suma = 0;
	int contador = 0;
	int last_val = 0;
	while(1){
		if(contador == 40){
			if(suma >= THRESHOLD_HIGH){
				if(last_val == 0 && (ptr->event == FALLIN_EDGE || ptr->event == CHANGE)){
					printf("Rising \n");
					ptr->callback(RISING_EDGE);
				}
				last_val = 1;
			}else if(suma <= THRESHOLD_LOW){
				if(last_val == 1 && (ptr->event == RISING_EDGE || ptr->event == CHANGE)){
					ptr->callback(FALLIN_EDGE);
					printf("Falling \n");
				}
				last_val = 0;
			}
			suma = 0;
			contador = 0;

		}
		suma += bcm2835_gpio_lev(pin);
		contador++;
		delay(5);
	}

}

void attachIsr(uint8_t PIN, uint8_t ISREvent, void* handdle, void* userData ){
    printf("Entro");
    fflush(stdout);
//    if (!bcm2835_init())
//        return ;
    // Set RPI pin P1-15 to be an input
    fflush(stdout);
    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
    //  with a pullup
    bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);
    // And a low detect enable
    ISR_Typ_* interrupt = (ISR_Typ_*)malloc(sizeof(ISR_Typ_));
    interrupt->callback = userData;
    interrupt->event = ISREvent;
    interrupt->pin = PIN;
//    switch(ISREvent){
//
//    case RISING_EDGE:
//    	bcm2835_gpio_ren(PIN);
//    	break;
//    case FALLIN_EDGE:
//      	bcm2835_gpio_fen(PIN);
//      	break;
//    case HIGH_DETECT:
//      	bcm2835_gpio_hen(PIN);
//      	break;
//    case LOW_DETECT:
//      	bcm2835_gpio_len(PIN);
//      	break;
//    case CHANGE:
//      	bcm2835_gpio_ren(PIN);
//      	bcm2835_gpio_fen(PIN);
//      	break;
//
//    default:
//    	break;
//    }
    printf("Entro");
    fflush(stdout);
    pthread_create(&thread,0x00, loop, interrupt);

}

