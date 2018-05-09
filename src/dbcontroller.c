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

#define SQL_QUERY_CREATE_DB "CREATE TABLE %s(USERID INT PRIMARY KEY     NOT NULL,SONG           TEXT    NOT NULL);"
#define SQL_QUERY_INSERT_DB "INSERT INTO %s (USERID,SONG) VALUES (%d,'%s'); "
#define SQL_QUERY_CHECK_DB "SELECT name FROM sqlite_master WHERE type='table' AND name='%s';"
#define SQL_QUERY_SELECT_DB "SELECT SONG FROM %s WHERE USERID = %d;"
#define SQL_TABLE_NAME "TARJETAS"
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


sqlite3* db_load(char* file_dir){
	sqlite3* db;
	int rc;
	if((rc = sqlite3_open(file_dir,&db))){
		fprintf(stderr,"No se puede abrir la base de datos %s \n",sqlite3_errmsg(db));
		return NULL;
	}
	fprintf(stdout,"Base de datos abierta \n");
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

void db_create_tables(sqlite3* db){
	int rc;
	char* sql = (char*)malloc(SQL_QUERY_MAX_SIZE*sizeof(char));
	char* zErrMsg = 0;
	//Formateamos la query con sprintf para permitir genericos
	sprintf(sql,SQL_QUERY_CREATE_DB,SQL_TABLE_NAME);
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Base de datos con datos, no creo tablas\n Por si acaso printeo el error %s \n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Base de datos nueva, creo las tablas\n");
	}
	free(sql);
}

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

int db_get_song_name(sqlite3* db,int user_id,char* song_name){
	char *zErrMsg = 0;
	int rc;
	char* sql = (char*) malloc(SQL_QUERY_MAX_SIZE * sizeof(char));

	sprintf(sql,SQL_QUERY_SELECT_DB,SQL_TABLE_NAME,user_id);

	rc = sqlite3_exec(db, sql, db_callback_get_song_name, song_name, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		free(sql);
		return SQL_OPERATION_ERR;
	} else {
		fprintf(stdout, "Lectura correcta de la base de datos\n");
	}
	free(sql);
	return SQL_OPERATION_OK;
}

void db_close(sqlite3* db){
	sqlite3_close(db);
}


/*
 * Callback Functions from SQLQuerys
 * */
static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);

   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");
   return 0;
}


static int db_callback_get_song_name(void *data_in, int argc, char **argv, char **azColName){
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}





