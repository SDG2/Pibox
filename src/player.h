/*
 * payer.h
 *
 *  Created on: 19 abr. 2018
 *      Author: Alejo
 */

#ifndef SRC_PLAYER_H_
#define SRC_PLAYER_H_

#include <stdio.h>
#include <stdlib.h>
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

// variables
enum fsm_states
{
	WAIT_BEGIN,
	WAIT_BFF1,
	WAIT_BFF2
};

//types
//strucutra de buffers
/**
 * @brief Estructura que almacena los buffers del Ping Pong
 * 
 */
typedef struct BUFFERS
{
	int *buff1_r; ///< Buffer1 del canal derecho
	int *buff2_r; ///< Buffer2 del canal derecho

	int *buff1_l; ///< Buffer1 del canal izquierdo
	int *buff2_l; ///< Buffer1 del canal izquierdo

	uint8_t currentBuffer; ///< Buffer actual
	int lengthBuffer; ///<Longitud de los buffers
	int sampleReaded; ///< Puntero de muestra

	volatile uint8_t flags; ///< Flags asociados al Buffer
} BUFFERS_T;
//Tipo fsm
/**
 * @brief Estructura de la maquina de estados player
 * 
 */
typedef struct fsm_audio_controller
{
	fsm_t *fsm; ///< Puntero comun de maquina de estados 
	TipoSistema *tipo_sistema; ///< Puntero a tipo sistema -> en desuso
	BUFFERS_T *buffer; ///< Puntero a los buffers
} fsm_audio_controller_t;

//prototypes
void launchPlayer();

#endif /* SRC_PLAYER_H_ */
