/*
 * tone.h
 *
 *  Created on: 12 mar. 2018
 *      Author: Alejo
 */

#ifndef SRC_TONE_H_
#define SRC_TONE_H_

#include "bcm2835.h"

#define PWM_CHANNEL 0
#define RANGE 1024
#define PWM_FREQ_BASE 1920000

extern void tone_init(uint8_t pin);
extern void tone_write(uint32_t freq);
extern void tone_stop(void);

#endif /* SRC_TONE_H_ */
