/*
 * fsm_rfid.c
 *
 *  Created on: 8 may. 2018
 *      Author: Alejo
 */



#include <stdio.h>
#include <dirent.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>


#include "dbcontroller.h"
#include "fsm_rfid.h"
#include "mutex.h"
#include "menu_lcd.h"
#include "InterruptSM.h"



#define DB_NAME "COMO_TE_SALGA.db"
#define PIN_A 17
#define PIN_B 27
#define PIN_C 22



void ConfiguraTarjeta(fsm_t* fsm);
void ConfiguracionCorrecta();
void BuscaTarjeta(fsm_t* fsm);
static int TarjetaNoExiste(fsm_t* fsm);
static int TarjetaExiste(fsm_t* fsm);
//static int TarjetaConfigurada(fsm_t* fsm);

int UUID_2_int();
void menu_display_stepper_plus(list_files_t* lista);
char* get_next_file(list_files_t* lista);
list_files_t* get_list_files(char* route);
list_files_t* new_list_files(int num_files);
int get_number_files(char* route);
void ISR (int event);
void select_mode(int event);

fsm_trans_t transition_table_rfid[] = {
		{WAIT_START, CompruebaComienzo,WAIT_CARD,ComienzaSistema},
		{WAIT_CARD,TarjetaDisponible,WAIT_CHECK, LeerTarjeta},
		{WAIT_CARD,TarjetaNoDisponible,WAIT_CARD,EsperaTargeta},
		{WAIT_CHECK,TarjetaNoValida,WAIT_CARD,DescartaTarjeta},
		{WAIT_CHECK,TarjetaValida,WAIT_DATA,BuscaTarjeta},
		{WAIT_DATA,TarjetaNoExiste,WAIT_PLAY,ConfiguraTarjeta},
		{WAIT_DATA,TarjetaExiste,WAIT_PLAY,ComienzaReproduccion},
		{WAIT_PLAY,TarjetaDisponible,WAIT_PLAY,CompruebaTarjeta},
		{WAIT_PLAY,TarjetaNoDisponible,WAIT_START,CancelaReproduccion},
		{WAIT_PLAY,CompruebaFinalReproduccion,WAIT_START,FinalizaReproduccion},
		{-1, NULL, -1, NULL }
};

static int TarjetaConfigurada(fsm_t* fsm){
	return 1;
}

static int TarjetaNoExiste(fsm_t* fsm){
	lock(0);
	uint8_t tmp =  !(flag_rfid & FLAG_CARD_EXIST);
	unlock(0);
	//printf("Tarjeta No Existe flag %d \n",flag_rfid);
	return tmp;
}

static int TarjetaExiste(fsm_t* fsm){
	lock(0);
	uint8_t tmp = (flag_rfid & FLAG_CARD_EXIST);
	unlock(0);
	//printf("Tarjeta Existe flag %d \n",flag_rfid);
	return tmp;
}


int CompruebaComienzo(fsm_t* fsm){
	return 1;
}

int CompruebaFinalReproduccion(fsm_t* fsm){
	return flag_rfid & FLAG_SYSTEM_END;
}

int TarjetaDisponible(fsm_t* fsm){
	return flag_rfid & FLAG_CARD_IN;
	//return 1;
}
int TarjetaNoDisponible(fsm_t* fsm){
	return !(flag_rfid & FLAG_CARD_IN);
	//return 0;
}
int TarjetaValida(fsm_t* fsm){
	printf("Flag: %d \n", flag_rfid);
	return flag_rfid & FLAG_VALID_CARD;
}
int TarjetaNoValida(fsm_t* fsm){
	return !(flag_rfid & FLAG_VALID_CARD);
}

void EsperaTargeta(fsm_t* fsm){
	return;
}
void DescartaTarjeta(fsm_t* fsm){
	flag_rfid &= ~FLAG_CARD_IN;
}
void LeerTarjeta(fsm_t* fsm){
	if(RC522_Check(UUID) == STATUS_OK){
		printf("FOUND TAG! \n");
		fflush(stdout);
		flag_rfid |= FLAG_VALID_CARD;
		fflush(stdout);
	}

}
void ComienzaReproduccion(fsm_t* fsm){
	fflush(stdout);
	lock(0);
	flags_player = FLAG_START;
	unlock(0);
	printf("comienzas flag: %d", flags_player);

}
void CancelaReproduccion(fsm_t* fsm){
	lock(0);
	flags_player |= FLAG_END;
	unlock(0);
}
void CompruebaTarjeta(fsm_t* fsm){

}
void FinalizaReproduccion(fsm_t* fsm){
	lock(0);
	flags_player |= FLAG_END;
	unlock(0);
}
void ComienzaSistema(fsm_t* fsm){
	printf("Arranca el RFID");
	menu_lcd_display_clear(),
	menu_lcd_display("PiMusicBox Player!","","","");
	fflush(stdout);
	if(RC522_Init() == STATUS_ERROR)
		printf("ERROR! \n");
	flag_rfid = 0;
}

void lp(void* userData){
	fsm_t* data = (fsm_t*)userData;
	while(1){
		fsm_fire(data);
	}
}
void launchRFID(){
	fsm_rfid = fsm_new(transition_table_rfid,NULL);
	pthread_create(&thread,NULL, lp, fsm_rfid);
}

void BuscaTarjeta(fsm_t* fsm){
	sqlite3* db = db_load(DB_NAME);
	lock(5);
	printf("UUID: %d",UUID_2_int());
	song_name = db_get_song_name(db,UUID_2_int());
	printf("SALGO \n");
	delay(10);
	printf("SongName: %s",song_name);
	fflush(stdout);
	if(song_name == NULL){
		unlock(5);
		printf("Error con los datos de la db \n");
		flag_rfid |= ~FLAG_CARD_EXIST;
		return;
	}
	unlock(5);
	flag_rfid |= FLAG_CARD_EXIST;
	menu_lcd_display("","Playing: ", song_name,"");
	db_close(db);
}

void ConfiguraTarjeta(fsm_t* fsm){

	printf("DATA -> PLAY \n");
	printf("Configuración de Tarjeta \n");

	attachIsr(PIN_A, CHANGE, NULL, ISR);
	attachIsr(PIN_B, CHANGE, NULL, ISR);
	attachIsr(PIN_C, FALLIN_EDGE, NULL, select_mode);

	menu_lcd_display("NOT CONFIGURED!","turn to ","configure",":D");


	stepper_irq_flag = 0;
	list_files_t* lista =  get_list_files("./musica");
	while(!(stepper_irq_flag & FLAG_IRQ_STEPPER_CONTINUE));
	stepper_irq_flag = 0;
	menu_display_stepper_plus(lista);
	while(!(stepper_irq_flag & FLAG_IRQ_STEPPER_SELECT)){
		lock(7);
		if(stepper_irq_flag & FLAG_IRQ_STEPPER_CONTINUE){
			lista->select_file++;
			if(lista->select_file > lista->num_files){
				lista->select_file = 1;
			}
			menu_display_stepper_plus(lista);
			stepper_irq_flag &= ~FLAG_IRQ_STEPPER_CONTINUE;

			printf("Fichero %d",lista->select_file);
			fflush(stdout);
		}
		unlock(7);
	}

	sqlite3* db = db_load(DB_NAME);
	db_insert(db,UUID_2_int(),lista->name_file[lista->current_file-1]);
	ConfiguracionCorrecta();
	db_close(db);
}

void ConfiguracionCorrecta(){
	printf("Configuracion Correcta \n");
	deleteIsr(PIN_A);
	deleteIsr(PIN_B);
	deleteIsr(PIN_C);
	menu_lcd_display_clear();
	menu_lcd_display("Finalizado!"," Retire la","tarjeta",":D");
}

void killRFID(){
	pthread_cancel (thread) ;
}



void menu_display_stepper_plus(list_files_t* lista){
	menu_lcd_display_clear();
	if(stepper_irq_flag & FLAG_IRQ_STEPPER_DIR)
		lista->current_file-=2;


	printf("Posicion %d \n",lista->current_file);
	fflush(stdout);
	char line1[20] = ">";
	strcat(line1,get_next_file(lista));
	int nextIndex = lista->current_file;
	menu_lcd_display(line1,get_next_file(lista),get_next_file(lista),get_next_file(lista));
	lista->current_file = nextIndex;

}


list_files_t* get_list_files(char* route){
	DIR *dir;
	struct dirent *ent;
	list_files_t* lista;
	int num_files = get_number_files(route);
	printf("nfiles = %d \n", num_files);
	lista = new_list_files(num_files);
	int i = 0;
 	if ((dir = opendir (route)) != NULL) {
 	  /* print all the files and directories within directory */
 	  while ((ent = readdir (dir)) != NULL) {
 		 if (ent->d_type == DT_REG) {
			printf ("%s\n", ent->d_name);
			strcpy(lista->name_file[i++],ent->d_name);
 		 }
 	  }
 	  closedir(dir);
 	} else {
 		printf("Error no se puede abrir el directorio");
}
 	printf("all ok \n");
	return lista;
}

char* get_next_file(list_files_t* lista){


	if(lista->current_file >=  lista->num_files ||  lista->current_file < 0)
		lista->current_file = 0;

	printf("current pos: %d \n",lista->current_file);
	return lista->name_file[lista->current_file++];
}
char* get_last_file(list_files_t* lista){
	return lista->current_file >= 0 ? lista->name_file[lista->current_file--] : NULL;
}

int get_number_files(char* route){
	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	dirp = opendir(route); /* There should be error handling after this */
	while ((entry = readdir(dirp)) != NULL) {
	    if (entry->d_type == DT_REG) { /* If the entry is a regular file */
	         file_count++;
	    }
	}
	closedir(dirp);
	return file_count;
}

list_files_t* new_list_files(int num_files){
	int i = 0;
	list_files_t* lista = (list_files_t*)malloc(sizeof(list_files_t));
	lista->num_files = num_files;
	lista->current_file = 0;
	lista->select_file = 1;
	lista->name_file = (char**)malloc(sizeof(char*)*num_files);
	for(i = 0; i < num_files; i++){
		lista->name_file[i] = (char*)malloc(sizeof(char*)*20);
	}
	return lista;
}

void clean_list_files(list_files_t* lista){
	int i = 0;
	for(i = 0; i < lista->num_files; i++){
		free(lista->name_file[i]);
	}
	free(lista->name_file);
	free(lista);
}

int UUID_2_int(){
	unsigned int id;
	int i = 0;
	for(i = 0; i < 8; i++){
		id = (id<<8) | UUID[i];
	}
	return id;
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

void select_mode(int event){

	lock(7);
	stepper_irq_flag |= FLAG_IRQ_STEPPER_SELECT;
	unlock(7);

}
