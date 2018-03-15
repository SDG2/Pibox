/*
 * tone.c
 *
 *  Created on: 12 mar. 2018
 *      Author: Alejo
 */


#include "tone.h"
#include <stdio.h>

/*
 * WIP -> Funcion que emula la funcionalidad de Softtone
 * usando la libreria propia del soc de la raspberry
 * */


tmr_t *timer;
uint8_t PIN;
uint8_t state;
int cmd = 0;

/*
 * Inicializa el modulo
 * */

void tone_init(uint8_t P){
	PIN = P;
	state = 1;
	timer = tmr_new(toggle);
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
}

/*
 * Arranca el PWM con la frecuencia deseada
 * */

void tone_write(uint32_t freq){
	int tmp;
	if(freq == 0){
		tone_stop();
		return;
	}
	tmp = 500000 / freq;
	printf("tmp :%d \n",tmp);
	//tmr_startus_period(timer,tmp);
	printf("CMD : ---- %d \n",cmd);
	fflush(stdout);
	tmr_startus_period(timer,1000000);
}

/*
 * Stop del PWM
 * */
void tone_stop(){
	tmr_startms_period(timer, 0);
}

/*
 * Callback toggle del timer periodico que genera el pwm
 * */

void toggle(){
	printf("TOGGLE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!. \n");
	fflush(stdout);
	cmd++;

    bcm2835_gpio_write(PIN, state);
    state = !state;
}
