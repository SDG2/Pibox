/*
 * tone.c
 *
 *  Created on: 12 mar. 2018
 *      Author: Alejo
 */


#include "tone.h"

void tone_init(uint8_t PIN){
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5);
    bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 0);
    bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);
    bcm2835_pwm_set_data(PWM_CHANNEL, RANGE/2); //ciclo de trabajo del 50%
}

void tone_write(uint32_t freq){
	bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 0);
	if(!freq){
		return;
	}
	uint32_t divisor = (uint32_t)(PWM_FREQ_BASE / freq);
	bcm2835_pwm_set_clock(divisor);
	bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 0);
}
void tone_stop(void){
	bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 0);
}
