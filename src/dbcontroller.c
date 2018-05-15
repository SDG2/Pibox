/*
 * dbcontroller.c
 *
 *  Created on: 9 may. 2018
 *      Author: Calata
 */
#include "dbcontroller.h"
#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
 * Plantillas de Querys SQL usadas en las consultas a la base de datos
 * */
#define SQL_QUERY_CREATE_DB "CREATE TABLE %s(USERID INT PRIMARY KEY     NOT NULL,SONG           TEXT    NOT NULL);"
#define SQL_QUERY_INSERT_DB "INSERT INTO %s (USERID,SONG) VALUES (%d,'%s'); "
#define SQL_QUERY_CHECK_DB "SELECT name FROM sqlite_master WHERE type='table' AND name='%s';"
#define SQL_QUERY_SELECT_DB "SELECT SONG FROM %s WHERE USERID = %d;"
#define SQL_QUERY_GET_NUMBER_OF_ELEMENTS "SELECT count(*) FROM %s WHERE USERID = %d;"
/*Prototipo del nombre de la tabla a consultar*/
#define SQL_TABLE_NAME "TARJETAS"
/*Tamaño maximo de query que admite la librería de conexion*/
#define SQL_QUERY_MAX_SIZE 500

/*
 * Local Function prototypes
 * */

void db_create_tables(sqlite3* db);
/*
 * Local callbacks prototypes
 * */
static int callback(void *data, int argc, char **argv, char **azColName);
static int db_callback_get_song_name(void *data_in, int argc, char **argv, char **azColName);
static int db_callback_get_num_of_elm(void *data_in, int argc, char **argv, char **azColName);

/* Carga la base de datos en memoria
 * Devuelve un puntero al manejador de la misma
 * */
sqlite3* db_load(char* file_dir){
	sqlite3* db;
	int rc;
	if((rc = sqlite3_open(file_dir,&db))){
		fprintf(stderr,"No se puede abrir la base de datos %s \n",sqlite3_errmsg(db));
		return NULL;
	}
	fprintf(stdout,"Base de datos abierta \n");
	/*Funcion de persistencia para que siempre que haya una base de datos
	 * se compruebe que sea correcta
	 * */
	db_create_tables(db);
	return db;
}

int db_check(sqlite3* db){
	int rc;
	char* sql = (char*)malloc(SQL_QUERY_MAX_SIZE*sizeof(char));
	char* zErrMsg = 0;
	const char* data = "Callback function called";
	sprintf(sql,SQL_QUERY_CHECK_DB,SQL_TABLE_NAME);

	rc = sqlite3_exec(db, sql, callback, (void*) data, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Error query busqueda tablas: %s", zErrMsg);
		sqlite3_free(zErrMsg);
		return SQL_OPERATION_ERR;
	} else {
		fprintf(stdout, "Todo correcto buscando tablas\n");
	}
	free(sql);
	return SQL_OPERATION_OK;
}
/*
 * Comprueba si es una base de datos nueva
 * y en caso de serlo crea las tablas necesarias
 * */
void db_create_tables(sqlite3* db){
	int rc;
	char* sql = (char*)malloc(SQL_QUERY_MAX_SIZE*sizeof(char));
	char* zErrMsg = 0;
	//Formateamos la query con sprintf para permitir genericos
	sprintf(sql,SQL_QUERY_CREATE_DB,SQL_TABLE_NAME);
	/*
	 * Si la query me da error, es porque esa tabla ya existe,
	 * por lo que la creacion se ignora y no afecta a los datos
	 * */
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		//Si tengo error, la base ya tiene la tabla
		fprintf(stderr, "Base de datos con datos, no creo tablas\n Por si acaso printeo el error %s \n", zErrMsg);
		//Libero memoria del error
		sqlite3_free(zErrMsg);
	} else {
		/* Si no tengo error al inicializar la base de datos
		 * he creado una nueva, por lo que debo formatear las tablas
		 * acorde a lo esperado por la tarjeta
		 */
		fprintf(stdout, "Base de datos nueva, creo las tablas\n");
	}
	//Libero memoria de la query
	free(sql);
}

/*
 * Ejecuta una query de
 * */
int db_insert(sqlite3* db,int user_id,char* song_name){
	char *zErrMsg = 0;
	int rc;
	char* sql = (char*) malloc(SQL_QUERY_MAX_SIZE * sizeof(char));

	sprintf(sql,SQL_QUERY_INSERT_DB,SQL_TABLE_NAME,user_id,song_name);

	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		free(sql);
		return SQL_OPERATION_ERR;
	} else {
		fprintf(stdout, "Escritura correcta en la base de datos\n");
	}
	free(sql);
	return SQL_OPERATION_OK;
}

char* db_get_song_name(sqlite3* db,int user_id){
	char *zErrMsg = 0;
	int rc;
	char* sql = (char*) malloc(SQL_QUERY_MAX_SIZE * sizeof(char));
	char* name = (char*)malloc(sizeof(char)*200);
	sprintf(sql,SQL_QUERY_GET_NUMBER_OF_ELEMENTS,SQL_TABLE_NAME,user_id);
	printf("%s",sql);
	rc = sqlite3_exec(db, sql, db_callback_get_num_of_elm, name, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		free(sql);
		return NULL;
	}
	fprintf(stdout, "Lectura correcta de la base de datos\n");
	if(*(name) == 0x30){
		free(name);
		return NULL;
	}
	sprintf(sql,SQL_QUERY_SELECT_DB,SQL_TABLE_NAME,user_id);
	rc = sqlite3_exec(db, sql, db_callback_get_song_name, name, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		free(sql);
		return NULL;
	}
	fprintf(stdout, "Lectura correcta de la base de datos\n");
	free(sql);
	return name;
}

void db_free_song_name(char* name){
	free(name);
}

void db_close(sqlite3* db){
	sqlite3_close(db);
}


/*
 * Callback Functions from SQLQuerys
 * */


static int db_callback_get_num_of_elm(void *data_in, int argc, char **argv, char **azColName){
	char* name =  (char*)data_in;
	printf("Entro en cb de contar %d",*(argv[0]));
	if(*(argv[0]) == 0x30){
		strcpy(name,"0");
		printf("Name %s",name);
		return 0;
	}
	strcpy(name,"1");
	printf("NameB %s",name);
   return 0;
}

static int db_callback_get_song_name(void *data_in, int argc, char **argv, char **azColName){
   char* name =  (char*)data_in;
   printf("Entro en la callback de la db \n");
   fflush(stdout);
   strcpy(name,argv[0]);
   printf("La cancion es: %s",name);
   fflush(stdout);
   return 0;
}





