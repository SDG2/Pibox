/*
 * tone.h
 *
 *  Created on: 12 mar. 2018
 *      Author: Alejo
 */

#ifndef SRC_TONE_H_
#define SRC_TONE_H_

#include <stdio.h>
#include <pthread.h>
#include <sched.h>

#include "tmr.h"

#include "bcm2835.h"

extern void tone_init(uint8_t P);
extern void tone_write(uint32_t freq);
extern void tone_stop(void);

void bip(int pin, int freq);

#endif /* SRC_TONE_H_ */
