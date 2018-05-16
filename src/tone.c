/*
 * tone.c
 *
 *  Created on: 12 mar. 2018
 *      Author: Alejo
 */

#include "tone.h"

void toggle();
void bipCallBack();



tmr_t* keyTimer;
uint8_t success;
/*
 * WIP -> Funcion que emula la funcionalidad de Softtone
 * usando la libreria propia del soc de la raspberry
 *
 * */

//thread asociado
static pthread_t pwm_thread;

//Frecuencia asociada
static uint32_t pin_freq;

volatile uint8_t PIN;

/*
 * Inicializa el modulo
 * */

void tone_init(uint8_t P)
{
	if (P > 63)
	{
		printf("ERROR!. Pin %d out of bounds!", PIN);
		return;
	}
	PIN = P;
	if (pwm_thread != 0)
	{
		printf("Can not create Tone.!, had it been created yet?");
		return;
	}
	pthread_attr_t tattr;
	int ret;
	int newprio = 30;
	struct sched_param param;
	/* initialized with default attributes */
	ret = pthread_attr_init(&tattr);

	/* safe to get existing scheduling param */
	ret = pthread_attr_getschedparam(&tattr, &param);

	/* set the priority; others are unchanged */
	param.sched_priority = newprio;

	/* setting the new scheduling param */
	ret = pthread_attr_setschedparam(&tattr, &param);

	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
	printf("\n PIN: %d \n", PIN);
	pthread_create(&pwm_thread, &tattr, toggle, NULL);
}

/*
 * Arranca el PWM con la frecuencia deseada
 * */

void tone_write(uint32_t freq)
{
	if (freq < 0)
		freq = 0;
	else if (freq > 5000) // Max 5KHz
		freq = 5000;

	pin_freq = freq;
}

void tone_stop(void)
{
	if (pwm_thread != 0)
	{
		pthread_cancel(pwm_thread);
		bcm2835_gpio_write(PIN, LOW);
	}
	pwm_thread = 0;
}

/*
 * Callback ,toggle del timer periodico que genera el pwm
 *
 * */

void toggle()
{
	uint32_t halfPeriod;
	uint64_t tmp;
	while (1)
	{
		if (pin_freq == 0)
			bcm2835_delay(1);
		else
		{
			halfPeriod = 500000 / pin_freq;
			tmp = bcm2835_st_read();
			bcm2835_gpio_write(PIN, HIGH);
			bcm2835_delayMicroseconds(halfPeriod);
			bcm2835_gpio_write(PIN, LOW);
			bcm2835_delayMicroseconds(halfPeriod);
		}
	}
}

void bip(int pin, int freq){
	success = 0;
	printf("bip \n");
	keyTimer = tmr_new(bipCallBack);
	tone_init(pin);
	tone_write(freq);
	tmr_startms(keyTimer, 200);
	while(!success);
}

void bipCallBack(){
	printf("killed bip! \n");
	tone_stop();
	tmr_destroy(keyTimer);
	success = 1;

}
