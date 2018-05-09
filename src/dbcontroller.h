/*
 * dbcontroller.h
 *
 *  Created on: 9 may. 2018
 *      Author: Calata
 */

#ifndef DBCONTROLLER_H_
#define DBCONTROLLER_H_

#include <sqlite3.h>

enum SQL_RESOLUTION{
	SQL_OPERATION_OK,
	SQL_OPERATION_ERR
};


sqlite3* db_load(char* file_dir);
int db_check(sqlite3* db);
int db_insert(sqlite3* db,int user_id,char* song_name);
int db_get_song_name(sqlite3* db,int user_id,char* song_name);
void db_close(sqlite3* db);
#endif /* DBCONTROLLER_H_ */
