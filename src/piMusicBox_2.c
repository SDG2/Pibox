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

//Testing
#include  "dbcontroller.h"
#include <sqlite3.h>
void callback(int event);

extern fsm_trans_t transition_table_polla[];

void func(void* data){
	fsm_audio_controller_t* sFsm = (fsm_audio_controller_t*)data;
	while (1) {
			fsm_fire(sFsm->fsm);}
}

int main(void){
	char* song_name = NULL;
	printf("Hola Funciono");
	sqlite3* db = db_load("KK.db");
	db_get_song_name(db,5,song_name);
	db_close(db);
	pthread_attr_t tattr;
	pthread_t thread;
	fsm_audio_controller_t* sFsm = (fsm_audio_controller_t*)malloc(sizeof(fsm_audio_controller_t));
	sFsm->fsm = fsm_new(transition_table_polla,NULL);

	uint8_t tmp = 0;
	uint8_t ID[16];
	int n, i;
	printf("hello world \n");
	//if(RC522_Init() == STATUS_ERROR)
	if(RC522_Init())
		return 1;

	int ret;
	int newprio = 99;
    struct sched_param param;
	/* initialized with default attributes */
	ret = pthread_attr_init (&tattr);

	/* safe to get existing scheduling param */
	ret = pthread_attr_getschedparam (&tattr, &param);

	/* set the priority; others are unchanged */
	param.sched_priority = newprio;

	/* setting the new scheduling param */
	ret = pthread_attr_setschedparam (&tattr, &param);
	pthread_create(&thread,&tattr, func, sFsm);
	//func();
	menu_lcd_init();
	while (1){
		menu_lcd_display("hola","esto","funciona","de puta madre");
		delay(1000);
		menu_lcd_display_clear();
		delay(1000);
	}
	attachIsr(18, CHANGE, NULL, callback);
	launchRFID();
	while(1);
	/*while(1){
		if(RC522_Check(ID) == STATUS_OK){
			printf("found tag.! \n ID: ");
			for(i = 0; i < 16; i++){
				printf(" %d", ID[i]);
			}
			printf("\n");
			flags_player |= FLAG_START;
			RC522_Anticoll(ID);
		}
	}
*/
}
int k = 0;
void callback(int event){
	if(k == 0){
		k++;
		return;
	}
	fflush(stdout);
	if(event == FALLIN_EDGE){
		lock(1);
		flag_rfid |= FLAG_CARD_IN;
		unlock(1);
	}else if(event == RISING_EDGE){
		lock(1);
		flag_rfid &= ~FLAG_CARD_IN;
		unlock(1);
	}
}

