/*
 * fsm_rfid.c
 *
 *  Created on: 8 may. 2018
 *      Author: Alejo
 */


#include "fsm_rfid.h"
#include "mutex.h"
#include <stdio.h>
#include "dbcontroller.h"
#include <dirent.h>

#define DB_NAME "COMO_TE_SALGA.db"



void ConfiguraTarjeta(fsm_t* fsm);
void ConfiguracionCorrecta(fsm_t* fsm);
void BuscaTarjeta(fsm_t* fsm);
static int TarjetaNoExiste(fsm_t* fsm);
static int TarjetaExiste(fsm_t* fsm);
static int TarjetaConfigurada(fsm_t* fsm);

int UUID_2_int();

fsm_trans_t transition_table_rfid[] = {
		{WAIT_START, CompruebaComienzo,WAIT_CARD,ComienzaSistema},
		{WAIT_CARD,TarjetaDisponible,WAIT_CHECK, LeerTarjeta},
		{WAIT_CARD,TarjetaNoDisponible,WAIT_CARD,EsperaTargeta},
		{WAIT_CHECK,TarjetaNoValida,WAIT_CARD,DescartaTarjeta},
		{WAIT_CHECK,TarjetaValida,WAIT_DATA,BuscaTarjeta},
		{WAIT_DATA,TarjetaNoExiste,WAIT_PLAY,ConfiguraTarjeta},
		{WAIT_DATA,TarjetaExiste,WAIT_PLAY,ComienzaReproduccion},
		{WAIT_CHECK,TarjetaValida,WAIT_PLAY,ComienzaReproduccion},
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
	printf("Tarjeta No Existe flag %d \n",flag_rfid);
	return tmp;
}

static int TarjetaExiste(fsm_t* fsm){
	lock(0);
	uint8_t tmp = (flag_rfid & FLAG_CARD_EXIST);
	unlock(0);
	printf("Tarjeta Existe flag %d \n",flag_rfid);
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
	printf("LEE \n");
	if(RC522_Check(UUID) == STATUS_OK){
		printf("FOUND TAG! \n");
		flag_rfid |= FLAG_VALID_CARD;
	}
}
void ComienzaReproduccion(fsm_t* fsm){
	printf("eNTROOOO");
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
	printf("PENE: %s",song_name);
	fflush(stdout);
	if(song_name == NULL){
		unlock(5);
		printf("Error con los datos de la db \n");
		flag_rfid |= ~FLAG_CARD_EXIST;
		return;
	}
	unlock(5);
	flag_rfid |= FLAG_CARD_EXIST;
	db_close(db);
}

void ConfiguraTarjeta(fsm_t* fsm){
	printf("Configuración de Tarjeta \n");
	menu_lcd_display("Gire el Stepper","Para configurar","la tarjeta",":D");
	list_files_t* lista =  get_list_files("/");
	while(!(stepper_irq_flag & FLAG_IRQ_STEPPER_CONTINUE));
	stepper_irq_flag = 1;
	while(!(stepper_irq_flag & FLAG_IRQ_STEPPER_SELECT)){
		menu_display_stepper_plus(lista);
		while(!(stepper_irq_flag & FLAG_IRQ_STEPPER_CONTINUE));
	}
	sqlite3* db = db_load(DB_NAME);
	db_insert(db,UUID_2_int(),lista->name_file[num_file]);
	db_close(db);
}

void ConfiguracionCorrecta(fsm_t* fsm){
	printf("Configuracion Correcta \n");
	menu_lcd_display("Configuracion","Finalizada",":D",":D");
}

void killRFID(){

}

void menu_display_stepper_plus(list_files_t* lista){
	menu_lcd_display(get_next_file(lista),get_next_file(lista),get_next_file(lista),get_next_file(lista));
}

list_files_t* get_list_files(char* route){
	DIR *dir;
	struct dirent *ent;
	int i = 0;
	list_files_t* lista = new_list_files();
	if ((dir = opendir ("/")) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
	    printf ("%s\n", ent->d_name);
	    strcpy(lista->name_file[i++],ent->d_name);
	  }
	  closedir(dir);
	} else {
		printf("Error no se puede abrir el directorio");
	}
	return lista;
}

char* get_next_file(list_files_t* lista){
	if(lista->current_file < lista->num_files)
		return lista->name_file[lista->current_file++];
	lista->current_file = 0;
	return lista->name_file[lista->current_file++];
}
char* get_last_file(list_files_t* lista){
	return lista->current_file >= 0 ? lista->name_file[lista->current_file--] : NULL;
}

int get_number_files(char* route){
	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	dirp = opendir("path"); /* There should be error handling after this */
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
	int id;
	int i = 0;
	for(i = 0; i < 8; i++){
		id = (id<<8) | UUID[i];
	}
	return id;
}



