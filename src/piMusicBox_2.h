/*
 * piMusicBox_1.h
 *
 *  Created on: 11 de sep. de 2017
 *      Author: FFM
 */

#ifndef PIMUSICBOX_1_H_
#define PIMUSICBOX_1_H_

#ifdef DEBUG
#undef DEBUG
#endif

#define DEBUG 1



#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softTone.h>
#include <string.h>
#include <time.h>


#include "kbhit.h" // Para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas
#include "defines.h"
#include "tipos.h"
#include "timers.h"
#include "fsm_controller.h"

extern int frecuenciaDespacito[];
extern int tiempoDespacito[];
extern int frecuenciaGOT[];
extern int tiempoGOT[];
extern int frecuenciaTetris[];
extern int tiempoTetris[];
extern int frecuenciaStarwars[];
extern int tiempoStarwars[];



int InicializaMelodia (TipoMelodia *melodia, char *nombre, int *array_frecuencias, int *array_duraciones, int num_notas);
int systemSetup (void);
//void getUserOption();

#endif /* PIMUSICBOX_1_H_ */
