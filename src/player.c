/*
 * player.c
 *
 *  Created on: 19 abr. 2018
 *      Author: Alejo
 */

#include "player.h"
#include "mutex.h"
#include <string.h>

uint8_t createdyet = 0; ///<Variable que indica si la maquina de estados ha sido lanzada

static inline signed int scale(mad_fixed_t sample);
static int CompruebaPlayerStart(fsm_t *userData);
static int Comprueba_fin_bff2(fsm_t *userData);
static int Comprueba_fin_bff1(fsm_t *userData);
static int CompruebaFinal(fsm_t *userData);
static int patestCallback(const void *inputBuffer, void *outputBuffer,
						  unsigned long framesPerBuffer,
						  const PaStreamCallbackTimeInfo *timeInfo,
						  PaStreamCallbackFlags statusFlags,
						  void *userData);
void output(struct mad_pcm *pcm, BUFFERS_T *buffer, uint8_t cbuff);
void Iniciliza_player(fsm_t *userData);
void carga_bff1(fsm_t *userData);
void carga_bff2(fsm_t *userData);
BUFFERS_T *new_buffer(void);
void Final_Melodia(fsm_t *userData);
void func(void *data);

//local variables
struct mad_stream mad_stream; ///<Estructura de stream de audio
struct mad_frame mad_frame;   ///<Estructura de informacion del frame decodificado
struct mad_synth mad_synth;   ///<Estructura de sintesis de frame

PaStreamParameters outputParameters; ///<Estructura de parametros del driver de audio
PaStream *stream;					 ///<Estructura de Stream de reproduccion
PaError err;						 ///<Variable de error
FILE *fp;							 ///<Puntero a fichero. Contendrá el fichero .mp3

//Tabla de tansiciones
fsm_trans_t transition_table_player[] = {
	{WAIT_BEGIN, CompruebaPlayerStart, WAIT_BFF1, Iniciliza_player},
	{WAIT_BFF1, CompruebaFinal, WAIT_BEGIN, Final_Melodia},
	{WAIT_BFF1, Comprueba_fin_bff2, WAIT_BFF2, carga_bff2},
	{WAIT_BFF2, CompruebaFinal, WAIT_BEGIN, Final_Melodia},
	{WAIT_BFF2, Comprueba_fin_bff1, WAIT_BFF1, carga_bff1},
	{-1, NULL, -1, NULL}};

//Functions definitions

/**
 * @brief  Comprueba el flag de inicio
 * Compureba el flag de inicio de sistema FLAG_START
 * @param userData Estructura de datos de usuario 
 * @return int 	'1' si el flag esta activo, '0' si no esta activo
 */

static int CompruebaPlayerStart(fsm_t *userData)
{
	lock(1);
	int result = flags_player & FLAG_START;
	unlock(1);
	return result;
}
/**
 * @brief Comprueba el final del buffer n2
 * Comprueba el flag del final de buffer (FLAG_BFF2_END) y el flag de final del sistema (FLAG_END)
 * @param userData Estructura de datos de usuario
 * @return int '1' si alguno de los flags esta activo. '0' de lo contrario
 */
static int Comprueba_fin_bff2(fsm_t *userData)
{

	lock(1);
	int result = (flags_player & FLAG_BFF2_END) && ~(flags_player & FLAG_END);
	unlock(1);
	return result;
}
/**
 * @brief Comprueba el final del buffer n1
 * Comprueba el flag del final de buffer (FLAG_BFF1_END) y el flag de final del sistema (FLAG_END)
 * @param userData Estructura de datos de usuario
 * @return int '1' si alguno de los flags esta activo. '0' de lo contrario
 */

static int Comprueba_fin_bff1(fsm_t *userData)
{
	lock(1);
	int result = (flags_player & FLAG_BFF1_END) && ~(flags_player & FLAG_END);
	unlock(1);
	return result;
}
/**
 * @brief Comprueba el final del sistema
 * Comprueba el flag de final del sistema (FLAG_END)
 * @param userData Estructura de datos de usuario
 * @return int 	'1' si el flag esta activo, '0' si no esta activo
 */

static int CompruebaFinal(fsm_t *userData)
{

	lock(1);
	int result = (flags_player & FLAG_END);
	unlock(1);
	return result;
}

/**
 * @brief Inicia el las estrucuturas necesarias
 * Inicia los estructuras de decodificacion y reproduccion necesarias. Tambien carga el primer frame decodificado+
 * en el buffer 1
 * @param userData Estructura de datos de usuario
 */

void Iniciliza_player(fsm_t *userData)
{

	fsm_audio_controller_t *data = (fsm_audio_controller_t *)userData; //Cargo datos de usuario
	data->buffer = new_buffer();									   //Creo el la estructura de buffers
	flags_player &= ~FLAG_START;

	//Cargo el fichero seleccionado en memoria
	char filename[64] = "./musica/"; //ruta parcial dle fichero
	strcat(filename, song_name);
	printf("full path: %s \n", filename);
	free(song_name);

	fp = fopen(filename, "r");
	int fd = fileno(fp); //obtengo puntero

	struct stat metadata; //estructua de informacion de fichero

	//compruebo que se ha cargado correctamente
	if (fstat(fd, &metadata) >= 0)
	{
		printf("File size %d bytes\n", (int)metadata.st_size);
	}
	else
	{
		printf("Failed to stat %s\n", filename);
		fclose(fp);
		return;
	}
	//mapeo el fichero
	const unsigned char *input_stream = mmap(0, metadata.st_size, PROT_READ, MAP_SHARED, fd, 0);
	mad_stream_buffer(&mad_stream, input_stream, metadata.st_size);

	//CARGAMOS - PROCESAMOS BFF1
	mad_frame_decode(&mad_frame, &mad_stream);

	//sistesis de frame
	mad_synth_frame(&mad_synth, &mad_frame);
	output(&mad_synth.pcm, data->buffer, 0);
	flags_player |= FLAG_BFF2_END;

	//Iniciamos el driver de audio
	err = Pa_Initialize();
	if (err != paNoError)
		goto error;
	//Cargmaos los dispositivos de reproduccion disponibles del sistema. nos quedamos con el por defecto
	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (outputParameters.device == paNoDevice)
	{
		fprintf(stderr, "Error: No default output device.\n");
		goto error;
	}

	//Parametros de reproduccion
	outputParameters.channelCount = 2;		 // stereo
	outputParameters.sampleFormat = paInt32; // 32 bit (tipo de muestras )
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	//Abro el stream de repducción
	Pa_OpenStream(
		&stream,
		NULL, // no input
		&outputParameters,
		SAMPLE_RATE,	   //velocidad de muestreo
		FRAMES_PER_BUFFER, //Tamaño del frame, por defecto 1152 (especificacion mpeg layer 3)
		paClipOff,		   //sin solapamiento
		patestCallback,	//funcion de callback
		data->buffer);

	//iniciamos el stream de audio
	Pa_StartStream(stream);
	return;

error:
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return;
}
/**
 * @brief Carga el buffer 2
 * Decodifica y sintetisa un frame y lo almacena en el buffer 2
 * @param userData Estructura de datos de usuario
 */

void carga_bff2(fsm_t *userData)
{

	fsm_audio_controller_t *data = (fsm_audio_controller_t *)userData;
	flags_player &= ~FLAG_BFF2_END;
	mad_frame_decode(&mad_frame, &mad_stream);
	// Synthesize PCM data of frame
	mad_synth_frame(&mad_synth, &mad_frame);
	output(&mad_synth.pcm, data->buffer, 1);
}
/**
 * @brief Carga el buffer 1
 * Decodifica y sintetisa un frame y lo almacena en el buffer 1
 * @param userData Estructura de datos de usuario
 */
void carga_bff1(fsm_t *userData)
{
	fsm_audio_controller_t *data = (fsm_audio_controller_t *)userData;
	flags_player &= ~FLAG_BFF1_END;
	mad_frame_decode(&mad_frame, &mad_stream);
	// Synthesize PCM data of frame
	mad_synth_frame(&mad_synth, &mad_frame);
	output(&mad_synth.pcm, data->buffer, 0);
}

/**
 * @brief Finaliza la melodia
 * Limpia la estructuras utilizadas y cierra los ficheros
 * @param userData Estructura de datos de usuario
 */

void Final_Melodia(fsm_t *userData)
{
	printf("Final de la melodia \n");
	flags_player = 0;
	Pa_StopStream(stream);
	fclose(fp);

	// Free MAD structs
	mad_synth_finish(&mad_synth);
	mad_frame_finish(&mad_frame);
	mad_stream_finish(&mad_stream);
}

/**
 * @brief  Carga los frame en el buffer
 * Funcion auxiliar que carga los datos del frame en buffer que corresponda en el momento
 * @param pcm 	Estructura de datos con mas muestras pcm del frame ya sintetizadas
 * @param buffer Estructura del buffer de reproducción
 * @param cbuff  indice del buffer actual en el que hay que escribir 
 */

void output(struct mad_pcm *pcm, BUFFERS_T *buffer, uint8_t cbuff)
{
	register int nsamples = pcm->length;									   //longitud del frame
	mad_fixed_t const *left_ch = pcm->samples[0], *right_ch = pcm->samples[1]; //punteros a buffer pcm
	int i = 0;
	//cargo los buffers correspondientes a los dos canales
	while (nsamples--)
	{
		signed int sample;
		sample = *left_ch++;
		if (cbuff == 0)
			*(buffer->buff1_l + i) = sample;
		else
			*(buffer->buff2_l + i) = sample;

		sample = *right_ch++;
		if (cbuff == 0)
			*(buffer->buff1_r + i) = sample;
		else
			*(buffer->buff2_r + i) = sample;

		i++;
	}
}

/**
 * @brief Callback llamada cada vez que el stream de reproducción se queda sin muestras 
 * 	Carga los datos del buffer al buffer de salida del stream. Tambien de encarga de hacer un switch entre
 * los buffers (selecciona cual se carga el siguiente)
 * @param inputBuffer  Buffer de entrada (no usado)
 * @param outputBuffer 	Buffer de salida
 * @param framesPerBuffer longitud del frame
 * @param timeInfo 	Informacion del tiempo de reproducción (no usado)
 * @param statusFlags flags de estado (no usado)
 * @param userData 	Datos de usuario
 * @return int 	resultado de la funcion 
 */
static int patestCallback(const void *inputBuffer, void *outputBuffer,
						  unsigned long framesPerBuffer,
						  const PaStreamCallbackTimeInfo *timeInfo,
						  PaStreamCallbackFlags statusFlags,
						  void *userData)
{
	BUFFERS_T *buffer = (BUFFERS_T *)userData;
	int *out = (int *)outputBuffer;
	unsigned long i;
	/* Prevent unused variable warnings. */
	(void)timeInfo;
	(void)statusFlags;
	(void)inputBuffer;
	//Si el buffer actual es el 1, lo cargo
	if (buffer->currentBuffer == 1)
	{
		buffer->currentBuffer = 0; //cambio de buffer
		lock(1);
		flags_player |= FLAG_BFF2_END; //pongo el flag
		unlock(1);
	}
	else
	{
		buffer->currentBuffer = 1;
		lock(1);
		flags_player |= FLAG_BFF1_END;
		unlock(1);
	}
	int j = 0;
	//cargo las muestars
	for (i = 0; i < framesPerBuffer; i++)
	{

		if (buffer->currentBuffer == 0)
		{
			*out++ = *(buffer->buff1_l + i); /* right */
			*out++ = *(buffer->buff1_r + i);
		}
		else
		{
			*out++ = *(buffer->buff2_l + i); /* right */
			*out++ = *(buffer->buff2_r + i);
		}
		j += 2;
	}
	buffer->sampleReaded++;

	return paContinue;
}
/**
 * @brief Crea una estructura de buffers
 * 
 * @return BUFFERS_T*  Puntero a la estructura creada
 */

BUFFERS_T *new_buffer(void)
{
	BUFFERS_T *new = (BUFFERS_T *)malloc(sizeof(BUFFERS_T));
	new->buff1_l = (int *)malloc(sizeof(int) * FRAMES_PER_BUFFER);
	new->buff1_r = (int *)malloc(sizeof(int) * FRAMES_PER_BUFFER);
	new->buff2_l = (int *)malloc(sizeof(int) * FRAMES_PER_BUFFER);
	new->buff2_r = (int *)malloc(sizeof(int) * FRAMES_PER_BUFFER);
	new->currentBuffer = 1;
	new->lengthBuffer = FRAMES_PER_BUFFER;
	new->sampleReaded = 0;
	new->flags = 0;
	return new;
}

static inline signed int scale(mad_fixed_t sample) // @suppress("Unused static function")
{
	/* round */
	sample += (1L << (MAD_F_FRACBITS - 16));

	/* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}
/**
 * @brief inicia el reproductor de audio
 * Crea la maquina de estados y la lanza en un hilo de alta prioridad para garantizar tiempo real
 * 
 */
void launchPlayer()
{
	if(createdyet){
		printf("ya lanzado \n");
		return;
	}
	createdyet = 1;
	fsm_audio_controller_t *sFsm = (fsm_audio_controller_t *)malloc(sizeof(fsm_audio_controller_t));
	sFsm->fsm = fsm_new(transition_table_player, NULL);

	pthread_attr_t tattr;
	pthread_t thread;
	int newprio = 99;
	struct sched_param param;
	/* initialized with default attributes */
	pthread_attr_init(&tattr);

	/* safe to get existing scheduling param */
	pthread_attr_getschedparam(&tattr, &param);

	/* set the priority; others are unchanged */
	param.sched_priority = newprio;

	/* setting the new scheduling param */
	pthread_attr_setschedparam(&tattr, &param);
	pthread_create(&thread, &tattr, func, sFsm);
}

/**
 * @brief funcion de loop del fsm
 * 
 * @param data 
 */
void func(void *data)
{
	fsm_audio_controller_t *sFsm = (fsm_audio_controller_t *)data;
	while (1)
	{
		fsm_fire(sFsm->fsm);
	}
}
