/*
 * dbcontroller.h
 *
 *  Created on: 9 may. 2018
 *      Author: Calata
 */

#ifndef DBCONTROLLER_H_
#define DBCONTROLLER_H_

#include <sqlite3.h>


sqlite3* load_db(char* file_dir);
void check_db(sqlite3* db);
void create_tables_db(sqlite3* db);


#endif /* DBCONTROLLER_H_ */
