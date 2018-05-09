/*
 * dbcontroller.c
 *
 *  Created on: 9 may. 2018
 *      Author: Calata
 */
#include "dbcontroller.h"
#include <stdlib.h>
#include "stdio.h"

#define SQL_QUERY_CREATE_DB "CREATE TABLE TARJETAS("  \
	         "USERID INT PRIMARY KEY     NOT NULL," \
	         "SONG           TEXT    NOT NULL);";

#define SQL_QUERY_CHECK_DB "SELECT name FROM sqlite_master WHERE type='table' AND name='TARJETAS';"

static int callback(void *data, int argc, char **argv, char **azColName);

sqlite3* load_db(char* file_dir){
	sqlite3* db;
	int rc;
	if((rc = sqlite3_open("canciones.db",&db))){
		fprintf(stderr,"No se puede abrir la base de datos %s \n",sqlite3_errmsg(db));
		return NULL;
	}
	fprintf(stdout,"Base de datos abierta \n");
	return db;
}

void check_db(sqlite3* db){
	int rc;
	char* sql = SQL_QUERY_CHECK_DB;
	char* zErrMsg = 0;
	const char* data = "Callback function called";

	rc = sqlite3_exec(db, sql, callback, (void*) data, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Operation done successfully\n");
	}
}

void create_tables_db(sqlite3* db){
	int rc;
	char* sql = SQL_QUERY_CREATE_DB;
	char* zErrMsg = 0;
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Table created successfully\n");
	}
}

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);

   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }

   printf("\n");
   return 0;
}





