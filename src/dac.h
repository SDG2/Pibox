/*
 * dac.h
 *
 *  Created on: 5 abr. 2018
 *      Author: jcala
 */

#ifndef PIBOX_SRC_DAC_H_
#define PIBOX_SRC_DAC_H_

#define BUFF_LENGTH 1024

#include <stdint.h>

typedef struct DAC{
	int buffer0[BUFF_LENGTH];
	int buffer1[BUFF_LENGTH];
	int** buffer_pointer;
	uint8_t currentBuff;
	int pos;
	uint8_t addr;
}DAC;


#endif /* PIBOX_SRC_DAC_H_ */
