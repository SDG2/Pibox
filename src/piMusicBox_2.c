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
#include  "dbcontroller.h"
#include <time.h>


#define PIN_A 17
#define PIN_B 27
#define PIN_C 22
#define TIME_THRESHOLD 3000000
int k = 0;

void callback(int event);
void ISR (int event);
void select_mode(int event);
int main(void){

	attachIsr(18, CHANGE, NULL, callback);
	attachIsr(PIN_A, CHANGE, NULL, ISR);
	attachIsr(PIN_B, CHANGE, NULL, ISR);
	attachIsr(PIN_C, FALLIN_EDGE, NULL, select_mode);

	menu_lcd_init();
	menu_lcd_display("PiBox RFID player!", "","","");
	launchPlayer();
	launchRFID();
	while(1);

}

void callback(int event){
	if(k == 0){
		k++;
		return;
	}
	if(event == FALLIN_EDGE){
		printf("FALLING event");
		fflush(stdout);
		lock(1);
		flag_rfid |= FLAG_CARD_IN;
		unlock(1);
	}else if(event == RISING_EDGE){
		printf("RISING event");
		fflush(stdout);
		lock(1);
		flag_rfid &= ~FLAG_CARD_IN;
		unlock(1);
	}
}

clock_t last_time = 0;

void select_mode(int event){

	lock(7);
	stepper_irq_flag |= FLAG_IRQ_STEPPER_SELECT;
	unlock(7);

}

int seqA;
int seqB;

void ISR (int event) {

	int A_val = bcm2835_gpio_lev(PIN_A);
    int B_val = bcm2835_gpio_lev(PIN_B);

    seqA <<= 1;
    seqA |= A_val;

    seqB <<= 1;
    seqB |= B_val;

    seqA &= 0b00001111;
    seqB &= 0b00001111;

    if (seqA == 0b00001001 && seqB == 0b00000011) {
    	stepper_irq_flag |= FLAG_IRQ_STEPPER_CONTINUE;
    	stepper_irq_flag |= FLAG_IRQ_STEPPER_DIR;
    	printf("He girado \n");
    	fflush(stdout);
    }

    if (seqA == 0b00000011 && seqB == 0b00001001) {
    	stepper_irq_flag |= FLAG_IRQ_STEPPER_CONTINUE;
    	stepper_irq_flag &=~FLAG_IRQ_STEPPER_DIR;
    	printf("He girado \n");
    	fflush(stdout);
    }
}


