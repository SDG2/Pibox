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

#define DB_NAME "canciones_db.db"
#define PIN_A 17
#define PIN_B 27
#define PIN_C 22

/**
 * @brief Variable privada con el UUID de la ultima tarjeta
 */
uint8_t UUID[16];
/**
 * @brief Indica si la maquina de estados ha sido creada ya o no
 */
int maquina_creada = 0;

void ConfiguraTarjeta(fsm_t *fsm);
void ConfiguracionCorrecta();
void BuscaTarjeta(fsm_t *fsm);
static int TarjetaNoExiste(fsm_t *fsm);
static int TarjetaExiste(fsm_t *fsm);
//static int TarjetaConfigurada(fsm_t* fsm);

int UUID_2_int();
void menu_display_stepper_plus(list_files_t *lista);
char *get_next_file(list_files_t *lista);
list_files_t *get_list_files(char *route);
list_files_t *new_list_files(int num_files);
int get_number_files(char *route);
void ISR(int event);
void select_mode(int event);

fsm_trans_t transition_table_rfid[] = {
	{WAIT_START, CompruebaComienzo, WAIT_CARD, ComienzaSistema},
	{WAIT_CARD, TarjetaDisponible, WAIT_CHECK, LeerTarjeta},
	{WAIT_CARD, TarjetaNoDisponible, WAIT_CARD, EsperaTargeta},
	{WAIT_CHECK, TarjetaNoValida, WAIT_CARD, DescartaTarjeta},
	{WAIT_CHECK, TarjetaValida, WAIT_DATA, BuscaTarjeta},
	{WAIT_DATA, TarjetaNoExiste, WAIT_PLAY, ConfiguraTarjeta},
	{WAIT_DATA, TarjetaExiste, WAIT_PLAY, ComienzaReproduccion},
	{WAIT_PLAY, TarjetaDisponible, WAIT_PLAY, CompruebaTarjeta},
	{WAIT_PLAY, TarjetaNoDisponible, WAIT_START, CancelaReproduccion},
	{WAIT_PLAY, CompruebaFinalReproduccion, WAIT_START, FinalizaReproduccion},
	{-1, NULL, -1, NULL}};

//TODO: Eliminar
static int TarjetaConfigurada(fsm_t *fsm)
{
	return 1;
}

/**
 * @brief Condición de paso si no existe tarjeta en la base de datos
 * 
 * @param fsm Maquina de estados
 * @return int Condición cumplida o no
 */
static int TarjetaNoExiste(fsm_t *fsm)
{
	lock(0);
	uint8_t tmp = !(flag_rfid & FLAG_CARD_EXIST);
	unlock(0);
	//printf("Tarjeta No Existe flag %d \n",flag_rfid);
	return tmp;
}

/**
 * @brief Condición de paso si  existe tarjeta en la base de datos
 * 
 * @param fsm Maquina de estados
 * @return int Condición cumplida o no
 */
static int TarjetaExiste(fsm_t *fsm)
{
	lock(0);
	uint8_t tmp = (flag_rfid & FLAG_CARD_EXIST);
	unlock(0);
	return tmp;
}
/**
 * @brief Funcion de arranque del sistema devuelve siempre 1
 * 
 * @param fsm 
 * @return int, siempre 1 
 */
int CompruebaComienzo(fsm_t *fsm)
{
	return 1;
}
/**
 * @brief Condición de paso si  Ha acabado la reproducción de la cancion
 * 
 * @param fsm Maquina de estados
 * @return int Condición cumplida o no
 */
int CompruebaFinalReproduccion(fsm_t *fsm)
{
	lock(0);
	uint8_t tmp = (flag_rfid & FLAG_SYSTEM_END);
	unlock(0);
	return tmp;
}
/**
 * @brief Condición de paso si hay una tarjeta insertada en el optoacoplador
 * 
 * @param fsm Maquina de estados
 * @return int Condición cumplida o no
 */
int TarjetaDisponible(fsm_t *fsm)
{
	lock(0);
	uint8_t tmp = (flag_rfid & FLAG_CARD_IN);
	unlock(0);
	return tmp;
}
/**
 * @brief Condición de paso si no hay una tarjeta insertada en el optoacoplador
 * 
 * @param fsm Maquina de estados
 * @return int Condición cumplida o no
 */
int TarjetaNoDisponible(fsm_t *fsm)
{
	lock(0);
	uint8_t tmp = !(flag_rfid & FLAG_CARD_IN);
	unlock(0);
	return tmp;
}
/**
 * @brief Condición de paso si la tarjeta es RFID
 * 
 * @param fsm Maquina de estados
 * @return int Condición cumplida o no
 */
int TarjetaValida(fsm_t *fsm)
{
	lock(0);
	uint8_t tmp = flag_rfid & FLAG_VALID_CARD;
	unlock(0);
	return tmp;
}
/**
 * @brief Condición de paso si la tarjeta no es RFID
 * 
 * @param fsm Maquina de estados
 * @return int Condición cumplida o no
 */
int TarjetaNoValida(fsm_t *fsm)
{
	lock(0);
	uint8_t tmp = !(flag_rfid & FLAG_VALID_CARD);
	unlock(0);
	return tmp;
}
/**
 * @brief Funcion de espera de la tarjeta mientras no haya una en el optoacoplador
 * 
 * @param fsm Maquina de estados
 */
void EsperaTargeta(fsm_t *fsm)
{
	return;
}
/**
 * @brief Altera el flag de tarjeta insertada
 * 
 * @param fsm Maquina de estados
 */
void DescartaTarjeta(fsm_t *fsm)
{
	lock(0);
	flag_rfid &= ~FLAG_CARD_IN;
	unlock(0);
}
/**
 * @brief Lee la tarjeta, activa el flag de tarjeta valida
 * Lee la tarjeta, almacena en una variable privada del fichero
 * el UUID de la misma. Activa el flag de tarjeta valida
 * @param fsm Maquina de estados 
 */
void LeerTarjeta(fsm_t *fsm)
{
	if (RC522_Check(UUID) == STATUS_OK)
	{
		printf("FOUND TAG! \n");
		fflush(stdout);
		flag_rfid |= FLAG_VALID_CARD;
	}
}
/**
 * @brief Activa el flag de inicio de reproduccion de la maquina de estados de player
 * 
 *@param fsm Maquina de estados
 */
void ComienzaReproduccion(fsm_t *fsm)
{
	menu_lcd_display("", "Playing: ", song_name, "");
	lock(1);
	flags_player = FLAG_START;
	unlock(1);
	printf("comienzas flag: %d", flags_player);
}
/**
 * @brief Activa el flag de inicio de stop de la maquina de estados de player
 * 
 *@param fsm Maquina de estados
 */
void CancelaReproduccion(fsm_t *fsm)
{
	lock(1);
	flags_player |= FLAG_END;
	unlock(1);
}
/**
 * @brief Funcion de espera de comprueba tarjeta
 * 
 *@param fsm Maquina de estados
 */
void CompruebaTarjeta(fsm_t *fsm)
{
	return;
}
/**
 * @brief Funcion de finalizacion de reproduccion
 * 
 *@param fsm Maquina de estados
 */
void FinalizaReproduccion(fsm_t *fsm)
{
	lock(1);
	flags_player |= FLAG_END;
	unlock(1);
}
/**
 * @brief Funcion de Inicizalizacion del sistema
 * 
 *@param fsm Maquina de estados
 */
void ComienzaSistema(fsm_t *fsm)
{
	printf("Arranca el RFID");
	menu_lcd_display_clear(),
		menu_lcd_display("PiMusicBox Player!", "", "", "");
	fflush(stdout);
	//Si no es la primera inicialización no ocurre nada aqui
	if (RC522_Init() == STATUS_ERROR)
		printf("ERROR! \n");
	//Reset de todos los flags
	lock(0);
	flag_rfid = 0;
	unlock(0)
}
/**
 * @brief Funcion de loop infinito de la maquina de estados
 * 
 * @param userData Datos externos 
 */
void lp(void *userData)
{
	fsm_t *data = (fsm_t *)userData;
	while (1)
	{
		fsm_fire(data);
	}
}
/**
 * @brief Funcion de creacion de la maquina de estados
 * Si la maquina de estados ya esta creada no se crea otra
 */
void launchRFID()
{
	if (maquina_creada)
	{
		printf("La maquina de estados ya ha sido creada \n");
		fflush(stdout);
		return;
	}
	fsm_rfid = fsm_new(transition_table_rfid, NULL);
	pthread_create(&thread, NULL, lp, fsm_rfid);
	maquina_creada = 1;
}
/**
 * @brief Funcion de Busqueda de la tarjeta en la Db
 * Busca si existe la tarjeta en la base de datos, en funcion
 * de este resultado activa los flags de FLAG_CARD_EXIST o no.
 *@param fsm Maquina de estados
 */
void BuscaTarjeta(fsm_t *fsm)
{
	//Iniciamos la base de datos
	sqlite3 *db = db_load(DB_NAME);
	lock(5);
	printf("UUID: %d", UUID_2_int());
	//Busca el nombre en la base de datos
	song_name = db_get_song_name(db, UUID_2_int());
	printf("SALGO \n");
	delay(10);
	printf("SongName: %s", song_name);
	fflush(stdout);
	//Si el puntero song_name es nulo no existe en la DB
	if (song_name == NULL)
	{
		unlock(5);
		printf("Error con los datos de la db \n");
		//Activamos el flag de no existir
		lock(0);
		flag_rfid |= ~FLAG_CARD_EXIST;
		unlock(0);
		return;
	}
	unlock(5);
	//Activamos el flag de existir
	lock(0);
	flag_rfid |= FLAG_CARD_EXIST;
	unlock(0);
	//Display por el LCD
	db_close(db);
}
/**
 * @brief Configura la tarjeta en la base de datos
 * Muestra un menu por pantalla e inicializa las interrupciones del,
 * encoder para que funcionen solamente en este punto del programa
 *@param fsm Maquina de estados
 */
void ConfiguraTarjeta(fsm_t *fsm)
{

	printf("DATA -> PLAY \n");
	printf("Configuraci�n de Tarjeta \n");
	//Inicializa las interrupciones del Encoder
	attachIsr(PIN_A, CHANGE, NULL, ISR);
	attachIsr(PIN_B, CHANGE, NULL, ISR);
	attachIsr(PIN_C, FALLIN_EDGE, NULL, select_mode);
	//Imprime la captura inicial
	menu_lcd_display("NOT CONFIGURED!", "turn to ", "configure", ":D");

	stepper_irq_flag = 0;
	//Cargo lista de archivos del directorio con la musica
	list_files_t *lista = get_list_files("./musica");
	//Bloqueo ejecucion hasta que se mueva el encoder una vez
	while (!(stepper_irq_flag & FLAG_IRQ_STEPPER_CONTINUE))
		;
	//Limpio los flags
	lock(7);
	stepper_irq_flag = 0;
	unlock(7);
	//Muentra la lista por pantalla
	menu_display_stepper_plus(lista);
	//Bloque ejecucion hasta que  no hay una pulsacion
	while (!(stepper_irq_flag & FLAG_IRQ_STEPPER_SELECT))
	{
		lock(7);
		//Si detecto movimiento del encoder
		if (stepper_irq_flag & FLAG_IRQ_STEPPER_CONTINUE)
		{
			lista->select_file++;
			if (lista->select_file > lista->num_files)
			{
				lista->select_file = 1;
			}
			//Refresco el display
			menu_display_stepper_plus(lista);
			stepper_irq_flag &= ~FLAG_IRQ_STEPPER_CONTINUE;

			printf("Fichero %d", lista->select_file);
			fflush(stdout);
		}
		unlock(7);
	}
	//Cargo base de datos y printeo pantalla final
	sqlite3 *db = db_load(DB_NAME);
	//Inserto en la db
	db_insert(db, UUID_2_int(), lista->name_file[lista->current_file - 1]);
	ConfiguracionCorrecta();
	db_close(db);
}
/**
 * @brief Desinicializacion tras la configuracion de una tarjeta
 * Elimina todas las interrupciones asociadas al encoder e imprime 
 * por el lcd el mensaje de final de la configuracion
 */
void ConfiguracionCorrecta()
{
	printf("Configuracion Correcta \n");
	deleteIsr(PIN_A);
	deleteIsr(PIN_B);
	deleteIsr(PIN_C);
	menu_lcd_display_clear();
	menu_lcd_display("Finalizado!", " Retire la", "tarjeta", ":D");
}
/**
 * @brief Mata la maquina de estados del rfid
 * 
 */
void killRFID()
{
	pthread_cancel(thread);
	maquina_creada = 0;
}

/**
 * @brief Refresco del LCD con los parametros de la lista
 * Funcion de manejo del menu que se actualiza en funcion de lo que se haya
 * operado en la funcion lista, para poder mostrar un menu rotativo bidireccional
 * por el LCD. Anade en la primera linea del LCD el promt de seleccion
 * @param lista 
 */
void menu_display_stepper_plus(list_files_t *lista)
{
	menu_lcd_display_clear();
	if (stepper_irq_flag & FLAG_IRQ_STEPPER_DIR)
		lista->current_file -= 2;

	printf("Posicion %d \n", lista->current_file);
	fflush(stdout);
	char line1[20] = ">";
	strcat(line1, get_next_file(lista));
	int nextIndex = lista->current_file;
	menu_lcd_display(line1, get_next_file(lista), get_next_file(lista), get_next_file(lista));
	lista->current_file = nextIndex;
}

/**
 * @brief Crea una estrucutura de tipo lista en el directorio pasado como parametro
 * Reserva memoria para una estructura de tipo lista de ficheros y la rellena con los datos
 * del directorio que se esta escaneando.
 * @param route 
 * @return list_files_t* 
 */
list_files_t *get_list_files(char *route)
{
	DIR *dir;
	struct dirent *ent;
	list_files_t *lista;
	//Inicializo la estructuar de datos
	int num_files = get_number_files(route);
	printf("nfiles = %d \n", num_files);
	lista = new_list_files(num_files);
	int i = 0;
	//Abro el directorio de la ruta
	if ((dir = opendir(route)) != NULL)
	{
		//Se ejecuta mientras haya algun fichero no leido
		while ((ent = readdir(dir)) != NULL)
		{
			//Si es un tipo regular, fichero valido, lo añado a la lista
			if (ent->d_type == DT_REG)
			{
				printf("%s\n", ent->d_name);
				strcpy(lista->name_file[i++], ent->d_name);
			}
		}
		//Cierro el directorio
		closedir(dir);
	}
	else
	{
		printf("Error no se puede abrir el directorio");
		return NULL;
	}
	printf("all ok \n");
	return lista;
}
/**
 * @brief Se mueve una posicion hacia delante el punero de fichero actual
 * 
 * @param lista 
 * @return char* 
 */
char *get_next_file(list_files_t *lista)
{

	if (lista->current_file >= lista->num_files || lista->current_file < 0)
		lista->current_file = 0;
	printf("current pos: %d \n", lista->current_file);
	return lista->name_file[lista->current_file++];
}

/**
 * @brief Se mueve una posicion hacia atrás el punero de fichero actual
 * 
 * @param lista 
 * @return char* 
 */
char *get_last_file(list_files_t *lista)
{
	return lista->current_file >= 0 ? lista->name_file[lista->current_file--] : NULL;
}

/**
 * @brief Cuenta el numero de ficheros que hay en la ruta
 * 
 * @param route 
 * @return int 
 */
int get_number_files(char *route)
{
	int file_count = 0;
	DIR *dirp;
	struct dirent *entry;

	dirp = opendir(route); /* There should be error handling after this */
	while ((entry = readdir(dirp)) != NULL)
	{
		if (entry->d_type == DT_REG)
		{ /* If the entry is a regular file */
			file_count++;
		}
	}
	closedir(dirp);
	return file_count;
}
/**
 * @brief Devuelve una estructura con memoria reservada y parametros por defecto
 * 
 * @param num_files numero de elementos de la lista
 * @return list_files_t* puntero a la lista creada
 */
list_files_t *new_list_files(int num_files)
{
	int i = 0;
	list_files_t *lista = (list_files_t *)malloc(sizeof(list_files_t));
	lista->num_files = num_files;
	lista->current_file = 0;
	lista->select_file = 1;
	lista->name_file = (char **)malloc(sizeof(char *) * num_files);
	for (i = 0; i < num_files; i++)
	{
		lista->name_file[i] = (char *)malloc(sizeof(char *) * 20);
	}
	return lista;
}
/**
 * @brief Destructor de una lista creada
 * 
 * @param lista estructura que se quiere eliminar de la memoria 
 */
void clean_list_files(list_files_t *lista)
{
	int i = 0;
	for (i = 0; i < lista->num_files; i++)
	{
		free(lista->name_file[i]);
	}
	free(lista->name_file);
	free(lista);
}
/**
 * @brief Conversor de Int Array a int del UUID
 * 
 * @return int UUID en formato int
 */
int UUID_2_int()
{
	unsigned int id;
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		id = (id << 8) | UUID[i];
	}
	return id;
}


/*******************************************
* Funciones de callback de las interrupciones
********************************************/
int seqA; 	//Almacenamiento de las secuencias muestreada en A 
int seqB;	//Almacenamiento de las secuencias muestreada en B

/**
 * @brief Registra las interrupciones del Encoder
 * 
 * @param event evento que se ha producido
 */
void ISR(int event)
{
	//Muestreamos ambos canales
	int A_val = bcm2835_gpio_lev(PIN_A);
	int B_val = bcm2835_gpio_lev(PIN_B);
	//Almaceno los valores muestreados
	seqA <<= 1;
	seqA |= A_val;

	seqB <<= 1;
	seqB |= B_val;
	//Enmascaro los valores
	seqA &= 0b00001111;
	seqB &= 0b00001111;
	//Compruebo si la secuencia coincide con las de un giro a derecha
	if (seqA == 0b00001001 && seqB == 0b00000011)
	{
		stepper_irq_flag |= FLAG_IRQ_STEPPER_CONTINUE;
		stepper_irq_flag |= FLAG_IRQ_STEPPER_DIR;
		printf("He girado \n");
		fflush(stdout);
	}
	//Compruebo si la secuencia coincide con las de un giro a izquierda
	if (seqA == 0b00000011 && seqB == 0b00001001)
	{
		stepper_irq_flag |= FLAG_IRQ_STEPPER_CONTINUE;
		stepper_irq_flag &= ~FLAG_IRQ_STEPPER_DIR;
		printf("He girado \n");
		fflush(stdout);
	}
}
/**
 * @brief Callback de boton del encoder
 * Activa el flag de seleccion del encoder
 *  @param event evento que se ha producido
 */
void select_mode(int event)
{
	lock(7);
	stepper_irq_flag |= FLAG_IRQ_STEPPER_SELECT;
	unlock(7);
}
