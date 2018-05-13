/*
 * payer.h
 *
 *  Created on: 19 abr. 2018
 *      Author: Alejo
 */

#ifndef SRC_PLAYER_H_
#define SRC_PLAYER_H_

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sched.h>

#include "mad.h"
#include "portaudio.h"

#include "tipos.h"
#include "fsm.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 1152



enum fsm_pene{
	WAIT_BEGIN,
	WAIT_BFF1,
	WAIT_BFF2
};

typedef struct BUFFERS{
	int* buff1_r;
	int* buff2_r;

	int* buff1_l;
	int* buff2_l;

	uint8_t currentBuffer;
	int lengthBuffer;
	int sampleReaded;

	volatile uint8_t flags;
}BUFFERS_T;

typedef struct fsm_audio_controller{
	fsm_t* fsm;
	TipoSistema* tipo_sistema;
	BUFFERS_T* buffer;
}fsm_audio_controller_t;

void launchPlayer();


#endif /* SRC_PLAYER_H_ */
