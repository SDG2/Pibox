
#include "piMusicBox_1.h"
//TODO: PIN_PWM
#define PIN_PWM 0


int frecuenciaDespacito[160] = {0,1175,1109,988,740,740,740,740,740,740,988,988,988,988,880,988,784,0,784,784,784,784,784,988,988,988,988,1109,1175,880,0,880,880,880,880,880,1175,1175,1175,1175,1318,1318,1109,0,1175,1109,988,740,740,740,740,740,740,988,988,988,988,880,988,784,0,784,784,784,784,784,988,988,988,988,1109,1175,880,0,880,880,880,880,880,1175,1175,1175,1175,1318,1318,1109,0,1480,1318,1480,1318,1480,1318,1480,1318,1480,1318,1480,1568,1568,1175,0,1175,1568,1568,1568,0,1568,1760,1568,1480,0,1480,1480,1480,1760,1568,1480,1318,659,659,659,659,659,659,659,659,554,587,1480,1318,1480,1318,1480,1318,1480,1318,1480,1318,1480,1568,1568,1175,0,1175,1568,1568,1568,1568,1760,1568,1480,0,1480,1480,1480,1760,1568,1480,1318};
int tiempoDespacito[160] = {1200,600,600,300,300,150,150,150,150,150,150,150,150,300,150,300,343,112,150,150,150,150,150,150,150,150,300,150,300,300,150,150,150,150,150,150,150,150,150,300,150,300,800,300,600,600,300,300,150,150,150,150,150,150,150,150,300,150,300,343,112,150,150,150,150,150,150,150,150,300,150,300,300,150,150,150,150,150,150,150,150,150,300,150,300,450,1800,150,150,150,150,300,150,300,150,150,150,300,150,300,450,450,300,150,150,225,75,150,150,300,450,800,150,150,300,150,150,300,450,150,150,150,150,150,150,150,150,300,300,150,150,150,150,150,150,450,150,150,150,300,150,300,450,450,300,150,150,150,300,150,300,450,800,150,150,300,150,150,300,450};
int frecuenciaGOT[518] = {1568,0,1046,0,1244,0,1397,0,1568,0,1046,0,1244,0,1397,0,1175,0,1397,0,932,0,1244,0,1175,0,1397,0,932,0,1244,0,1175,0,1046,0,831,0,698,0,523,0,349,0,784,0,523,0,523,0,587,0,622,0,698,0,784,0,523,0,622,0,698,0,784,0,523,0,622,0,698,0,587,0,698,0,466,0,622,0,587,0,698,0,466,0,622,0,587,0,523,0,523,0,587,0,622,0,698,0,784,0,523,0,622,0,698,0,784,0,523,0,622,0,698,0,587,0,698,0,466,0,622,0,587,0,698,0,466,0,622,0,587,0,523,0,0,1568,0,0,1046,0,0,1244,0,0,1397,0,0,1568,0,0,1046,0,0,1244,0,0,1397,0,0,1175,0,587,0,622,0,587,0,523,0,587,0,784,0,880,0,932,0,1046,0,1175,0,0,1397,0,0,932,0,0,1244,0,0,1175,0,0,1397,0,0,932,0,0,1244,0,0,1175,0,0,1046,0,0,1568,0,0,1046,0,0,1244,0,0,1397,0,0,1568,0,0,1046,0,0,1244,0,0,1397,0,0,1175,0,880,0,784,0,932,0,1244,0,0,1397,0,0,932,0,0,1175,0,0,1244,0,0,1175,0,0,932,0,0,1046,0,0,2093,0,622,0,831,0,932,0,1046,0,622,0,831,0,1046,0,0,1865,0,622,0,784,0,831,0,932,0,622,0,784,0,932,0,0,1661,0,523,0,698,0,784,0,831,0,523,0,698,0,831,0,0,1568,0,1046,0,1244,0,1397,0,1568,0,1046,0,1244,0,1397,0,0,0,1661,0,1046,0,1175,0,1244,0,831,0,1175,0,1244,0,0,0,0,2489,0,0,0,0,2794,0,0,0,0,3136,0,0,2093,0,622,0,831,0,932,0,1046,0,622,0,831,0,1046,0,0,1865,0,622,0,784,0,831,0,932,0,622,0,784,0,932,0,0,1661,0,523,0,698,0,784,0,831,0,523,0,698,0,831,0,0,1568,0,1046,0,1244,0,1397,0,1568,0,1046,0,1244,0,1397,0,0,0,1661,0,1046,0,1175,0,1244,0,831,0,1175,0,1244,0,0,0,0,2489,0,1397,0,0,0,2350,0,0,0,2489,0,0,0,2350,0,0,0,0,2093,0,392,0,415,0,466,0,523,0,392,0,415,0,466,0,523,0,392,0,415,0,466,0,2093,0,1568,0,1661,0,1865,0,2093,0,1568,0,1661,0,1865,0,2093,0,1568,0,1661,0,1865};
int tiempoGOT[518] = {900,89,900,89,133,13,133,13,600,59,600,59,133,13,133,13,1400,1400,900,89,900,89,133,13,133,13,600,59,900,89,133,13,133,13,1200,116,267,28,267,28,267,28,900,89,900,89,1400,89,69,7,69,7,69,7,69,7,900,89,900,89,133,13,133,13,600,59,600,59,133,13,133,13,1800,1800,900,89,900,89,133,13,133,13,600,59,900,89,133,13,133,13,1200,2400,69,7,69,7,69,7,69,7,900,89,900,89,133,13,133,13,600,59,600,59,133,13,133,13,1800,1800,900,89,900,89,133,13,133,13,600,59,900,89,133,13,133,13,1200,2400,3600,900,89,900,900,89,900,133,13,150,133,13,150,600,59,600,600,59,600,133,13,150,133,13,150,1200,400,69,7,69,7,69,7,69,7,267,28,400,45,133,13,267,28,267,28,267,28,300,900,89,900,900,89,900,133,13,150,133,13,150,600,59,600,900,89,900,133,13,150,133,13,150,1200,1800,3600,900,89,900,900,89,900,133,13,150,133,13,150,600,59,600,600,59,600,133,13,150,133,13,150,1200,400,267,28,1200,400,133,13,133,13,150,900,89,900,900,89,900,600,59,600,267,28,300,600,59,600,267,28,300,1200,2400,3600,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,150,267,28,267,28,133,13,133,13,133,13,267,28,267,28,133,13,150,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,150,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,150,150,600,59,133,13,133,13,267,28,267,28,133,13,133,13,150,150,150,900,89,900,900,900,900,89,900,900,900,1200,2400,3600,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,150,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,150,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,150,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,150,150,600,59,133,13,133,13,267,28,267,28,133,13,133,13,150,150,150,600,212,133,13,150,150,267,28,300,300,400,45,450,450,133,13,150,150,150,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,267,28,267,28,133,13,2400,116,267,28,267,28,133,13,133,13,267,28,267,28,133,13,133,13,267,28,267,28,133,13,2400};
int frecuenciaTetris[55] = {1319,988,1047,1175,1047,988,880,880,1047,1319,1175,1047,988,988,1047,1175,1319,1047,880,880,0,1175,1397,1760,1568,1397,1319,1047,1319,1175,1047,988,988,1047,1175,1319,1047,880,880,0,659,523,587,494,523,440,415,659,523,587,494,523,659,880,831};
int tiempoTetris[55] = {450,225,225,450,225,225,450,225,225,450,225,225,450,225,225,450,450,450,450,450,675,450,225,450,225,225,675,225,450,225,225,450,225,225,450,450,450,450,450,450,900,900,900,900,900,900,1800,900,900,900,900,450,450,900,1800};
int frecuenciaStarwars[59] = {523,0,523,0,523,0,698,0,1046,0,0,880,0,784,0,1397,0,523,0,1760,0,0,880,0,784,0,1397,0,523,0,1760,0,0,880,0,784,0,1397,0,523,0,1760,0,0,880,0,1760,0,0,784,0,523,0,0,523,0,0,523,0};
int tiempoStarwars[59] = {134,134,134,134,134,134,536,134,536,134,134,134,134,134,134,536,134,402,134,134,429,357,134,134,134,134,536,134,402,134,134,429,357,134,134,134,134,536,134,402,134,134,429,357,134,134,134,429,357,1071,268,67,67,268,67,67,67,67,67};

//Prototipado de funciones

//------------------------------------------------------
// void InicializaMelodia (TipoMelodia *melodia)
//
// Descripcion: inicializa los parametros del objeto melodia.
// Ejemplo de uso:
//
// ...
//
// TipoMelodia melodia_demo;
//
// if ( InicializaMelodia (&melodia_demo, "STARWARS", frecuenciaStarwars, tiempoStarwars, 59) < 0 ) {
// 		printf("\n[ERROR!!!][InicializaMelodia]\n");
// 		fflush(stdout);
// 		}
//
// ...
//
//------------------------------------------------------
int InicializaMelodia (TipoMelodia *melodia, char *nombre, int *array_frecuencias, int *array_duraciones, int num_notas) {
	// A completar por el alumno...
	// ...
	//Reservamos memoria
	//melodia->nombre = (char*)malloc(sizeof(char)*strlen(nombre));
	//melodia->frecuencias = (int*)malloc(sizeof(int)*num_notas);
	//melodia->duraciones = (int*)malloc(sizeof(int)*num_notas);
	//memcpy(melodia->nombre,nombre,sizeof(char)*strlen(nombre));
	//memcpy(melodia->frecuencias,array_duraciones,sizeof(int)*num_notas));
	//memcpy(melodia->duraciones,array_duraciones,sizeof(int)*num_notas));
	melodia->nombre = nombre;
	melodia->duraciones = array_duraciones;
	melodia->frecuencias = array_frecuencias;
	melodia->num_notas = num_notas;
	return melodia->num_notas;
}
void InicializaPlayer(TipoPlayer *p_player){
	p_player->posicion_nota_actual = 0;
	p_player->frecuencia_nota_actual = p_player->melodia->frecuencias[0];
	p_player->duracion_nota_actual = p_player->melodia->duraciones[0];
}

void ActualizaPlayer(TipoPlayer *p_player){
	p_player->posicion_nota_actual = (p_player->posicion_nota_actual)+1;
	p_player->frecuencia_nota_actual = p_player->melodia->frecuencias[p_player->posicion_nota_actual];
	p_player->duracion_nota_actual = p_player->melodia->frecuencias[p_player->posicion_nota_actual];
	softToneWrite(PIN_PWM,p_player->frecuencia_nota_actual);
	delay(p_player->duracion_nota_actual);
}
void StopPlayer(TipoPlayer *p_player){
	softToneWrite(PIN_PWM,0);
}
//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------

// int systemSetup (void): procedimiento de configuracion del sistema.
// Realizará, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librerías (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones periódicas y sus correspondientes temporizadores,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
int systemSetup (void) {
	// A completar por el alumno...
	// ...
	wiringPiSetupPhys();
	softToneCreate(PIN_PWM);
	return 0;
}

int main ()
{
	int duracion;
	char *nombre = "nombre";
	TipoSistema* sistema = (TipoSistema*)malloc(sizeof(TipoSistema));
	sistema->player.melodia = (TipoMelodia*) malloc(sizeof(TipoMelodia));

	// Configuracion e inicializacion del sistema
	systemSetup();

	while (1) {
		if(kbhit()) { // Funcion que detecta si se ha producido pulsacion de tecla alguna
			sistema->teclaPulsada = kbread(); // Funcion que devuelve la tecla pulsada

			printf("\n[KBHIT][%c]\n", sistema->teclaPulsada);

			// Interpretacion de las pulsaciones para cada posible estado del sistema
			if( sistema->estado == WAIT_START ) { // Cualquier pulsacion da comienzo a la reproduccion...
				// Descomente ambas lineas y sustituya cada etiqueta XXXXXXX por lo que corresponda en cada caso...
				 duracion = InicializaMelodia(sistema->player.melodia,nombre,frecuenciaDespacito,tiempoDespacito,160);
				 InicializaPlayer(&sistema->player);
				 sistema->estado = WAIT_PUSH;
				 //DEBUG
#ifdef DEBUG
#if DEBUG
				 printf("Sistema iniciado: Duracion %d",duracion);
#endif
#endif
			}
			else if( sistema->estado == WAIT_END ) { // Cualquier nos devuelve al estado inicial...
				// Descomente ambas lineas y sustituya cada etiqueta XXXXXXX por lo que corresponda en cada caso...
				// XXXXXXX();
				 sistema->estado = WAIT_START;
#ifdef DEBUG
#if DEBUG
				 printf("Sistema Estado final");
#endif
#endif
			}
			else { // Si estamos jugando...
				switch(sistema->teclaPulsada) {
					case 's':
						// A completar por el alumno...
						ActualizaPlayer(&sistema->player);
#ifdef DEBUG
#if DEBUG
				 printf("Suena Melodia");
#endif
#endif
						break;

					case 't':
						// A completar por el alumno...

						break;

					case 'q':
						exit(0);
						break;

					default:
						printf("INVALID KEY!!!\n");
						break;
				}
			}
		}
	}
}
