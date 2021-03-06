#include "fsm.h"
#include "player.h"
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include "RC522.h"
#include "fsm_rfid.h"
#include "piMusicBox_2.h"
#include "InterruptSM.h"
#include "mutex.h"
#include "menu_lcd.h"
#include "dbcontroller.h"
#include <time.h>

int k = 0;

void callback(int event);

int main(void)
{

	attachIsr(18, CHANGE, NULL, callback);	//creo interrupcion para el optoacoplador

	menu_lcd_init();						//inicio la pantalla
	menu_lcd_display("PiBox RFID player!", "", "", "");
	launchPlayer();							//lanzo maquina de estados del player
	launchRFID();							//lanzo maquina de estados del rfid
	while (1)
		;
}
/**
 * @brief  Callback llamado cuando se produce una interrupción
 * 
 * @param event tipo de evento que lo ha causado
 */
void callback(int event)
{
	if (k == 0)
	{
		k++;
		return;
	}
	if (event == FALLIN_EDGE)
	{
		printf("FALLING event");
		fflush(stdout);
		lock(1);
		flag_rfid |= FLAG_CARD_IN;
		unlock(1);
	}
	else if (event == RISING_EDGE)
	{
		printf("RISING event");
		fflush(stdout);
		lock(1);
		flag_rfid &= ~FLAG_CARD_IN;
		unlock(1);
	}
}
