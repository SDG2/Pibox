/*
 * menu_c.c
 *
 *  Created on: 9 may. 2018
 *      Author: jcala
 */

#include "menu_lcd.h"
#include <stdio.h>
#include <string.h>

#define MENU_LCD_EMPTY_LINE ""
#define LINE_SIZE_CONDITION(x) (strlen(x) <= 20)
#define LINES_LENGTH_VALID(a,b,c,d) (LINE_SIZE_CONDITION(a) && LINE_SIZE_CONDITION(b) && LINE_SIZE_CONDITION(c) && LINE_SIZE_CONDITION(d))

/*Funciones de Middleware para escribir en la pantalla*/
void write_lines();

/*Variables privadas*/
lines_2_write_t* current_screen;

/*Init*/
void menu_lcd_init(){
	lcd_init();
	current_screen = (lines_2_write_t*)malloc(sizeof(current_screen));
}
/*DeInit*/
void menu_lcd_deinit(){
	free(current_screen);
}
/*Funcion de escritura principal*/
void write_lines(){
	//Se comprueba con null para ver si la estructura esta inicializada
	if (current_screen->line1 != NULL) {
		lcdLoc(LINE1);
		typeln(current_screen->line1);
	}
	if (current_screen->line2 != NULL) {
		lcdLoc(LINE2);
		typeln(current_screen->line2);
	}
	if (current_screen->line3 != NULL) {
		lcdLoc(LINE3);
		typeln(current_screen->line3);
	}
	if (current_screen->line4 != NULL) {
		lcdLoc(LINE4);
		typeln(current_screen->line4);
	}
}
void menu_lcd_display_clear(){
	ClrLcd();
	strcpy(current_screen->line1,MENU_LCD_EMPTY_LINE);
	strcpy(current_screen->line2,MENU_LCD_EMPTY_LINE);
	strcpy(current_screen->line3,MENU_LCD_EMPTY_LINE);
	strcpy(current_screen->line4,MENU_LCD_EMPTY_LINE);
}

void menu_lcd_display(char* line1,char* line2,char* line3, char* line4){
//	if(!LINES_LENGTH_VALID(line1,line2,line3,line4)){
//		printf("Error, una de las lineas es demasiado larga");
//		return;
//	}
//	strcpy(current_screen->line1,line1);
//	strcpy(current_screen->line2,line2);
//	strcpy(current_screen->line3,line3);
//	strcpy(current_screen->line4,line4);
//	write_lines();
	if (line1 != NULL) {
		lcdLoc(LINE1);
		typeln(line1);
	}
	if (line2 != NULL) {
		lcdLoc(LINE2);
		typeln(line2);
	}
	if (line3 != NULL) {
		lcdLoc(LINE3);
		typeln(line3);
	}
	if (line4 != NULL) {
		lcdLoc(LINE4);
		typeln(line4);
	}
}


