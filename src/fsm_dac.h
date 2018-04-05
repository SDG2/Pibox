



#include<bcm2835.h>
#include "dac.h"
#include <stdint.h>

typedef enum{
	PARADO = 0,
	MANDA = 1,
}_state_dac;




typedef struct dac_fsm{
		fsm_t fsm;
		DAC* dac;
}dac_fsm_t;
