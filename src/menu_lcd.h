/*
 * menu_c.h
 *
 *  Created on: 9 may. 2018
 *      Author: jcala
 */

#include "LCD.h"

/*
 * Estructura de datos para escribir mas facilmente en la pantalla
 * */
typedef struct lines_2_write{
	char line1[20];
	char line2[20];
	char line3[20];
	char line4[20];
}lines_2_write_t;

/*Init*/
void menu_lcd_init();
/*DeInit*/
void menu_lcd_deinit();
/*Write Func*/
void menu_lcd_display(char* line1,char* line2,char* line3, char* line4);
void menu_lcd_display_clear();
