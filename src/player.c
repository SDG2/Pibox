/*
 * player.c
 *
 *  Created on: 19 abr. 2018
 *      Author: Alejo
 */

#include "player.h"
#include "mutex.h"
#include <string.h>


static inline signed int scale(mad_fixed_t sample);
static int CompruebaPlayerStart(fsm_t* userData);
static int Comprueba_fin_bff2(fsm_t* userData);
static int Comprueba_fin_bff1(fsm_t* userData);
static int CompruebaFinal(fsm_t* userData);
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData );
void output(struct mad_pcm *pcm, BUFFERS_T* buffer, uint8_t cbuff);
void Iniciliza_player(fsm_t* userData);
void carga_bff1(fsm_t* userData);
void carga_bff2(fsm_t* userData);
BUFFERS_T* new_buffer(void);
void Final_Melodia(fsm_t* userData);
void func(void* data);


struct mad_stream mad_stream;
struct mad_frame mad_frame;
struct mad_synth mad_synth;

PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
FILE *fp;

fsm_trans_t transition_table_polla[] = {
		{ WAIT_BEGIN, CompruebaPlayerStart, WAIT_BFF1, Iniciliza_player },
		{ WAIT_BFF1, CompruebaFinal, WAIT_BEGIN, Final_Melodia },
		{ WAIT_BFF1, Comprueba_fin_bff2, WAIT_BFF2, carga_bff2 },
		{ WAIT_BFF2, CompruebaFinal, WAIT_BEGIN, Final_Melodia },
		{ WAIT_BFF2, Comprueba_fin_bff1, WAIT_BFF1,carga_bff1 },
		{ -1, NULL,-1, NULL } };

static int CompruebaPlayerStart(fsm_t* userData) {
	lock(1);
	int result = flags_player & FLAG_START;
	unlock(1);
	return result;
}

static int Comprueba_fin_bff2(fsm_t* userData) {

	lock(1);
	int result = (flags_player & FLAG_BFF2_END)
			&& ~(flags_player & FLAG_END);
	unlock(1);
	return result;
}

static int Comprueba_fin_bff1(fsm_t* userData) {
	lock(1);
	int result = (flags_player & FLAG_BFF1_END)
			&& ~(flags_player & FLAG_END);
	unlock(1);
	return result;
}

static int CompruebaFinal(fsm_t* userData) {

	lock(1);
	int result = (flags_player & FLAG_END);
	unlock(1);
	return result;
}

void Iniciliza_player(fsm_t* userData) {
	fsm_audio_controller_t* data = (fsm_audio_controller_t*) userData;
	data->buffer = new_buffer();
	flags_player&=~FLAG_START;

	char filename[64] = "./musica/";
	strcat(filename, song_name);
	printf("full path: %s \n", filename);
	free(song_name);

	fp = fopen(filename, "r");
	int fd = fileno(fp);

	struct stat metadata;

	if (fstat(fd, &metadata) >= 0) {
		printf("File size %d bytes\n", (int) metadata.st_size);
	} else {
		printf("Failed to stat %s\n", filename);
		fclose(fp);
		return;
	}

	const unsigned char *input_stream = mmap(0, metadata.st_size, PROT_READ, MAP_SHARED, fd, 0);
	mad_stream_buffer(&mad_stream, input_stream, metadata.st_size);

	//CARGAMOS - PROCESAMOS BFF1
	mad_frame_decode(&mad_frame, &mad_stream);

	// Synthesize PCM data of frame
	mad_synth_frame(&mad_synth, &mad_frame);
	output(&mad_synth.pcm, data->buffer, 0);
	flags_player |= FLAG_BFF2_END;


	err = Pa_Initialize();
	if (err != paNoError)
		goto error;

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		goto error;
	}

	outputParameters.channelCount = 2; /* stereo output */
	outputParameters.sampleFormat = paInt32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;


	  Pa_OpenStream(
	               &stream,
	               NULL, /* no input */
	               &outputParameters,
	               SAMPLE_RATE,
	               FRAMES_PER_BUFFER,
	               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
	               patestCallback,
	               data->buffer);
	Pa_StartStream(stream);
	return;

	error:
	Pa_Terminate();
	fprintf( stderr, "An error occured while using the portaudio stream\n");
	fprintf( stderr, "Error number: %d\n", err);
	fprintf( stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return;

}


void carga_bff2(fsm_t* userData) {

	fsm_audio_controller_t* data = (fsm_audio_controller_t*) userData;
	flags_player&=~FLAG_BFF2_END;
	mad_frame_decode(&mad_frame, &mad_stream);
	// Synthesize PCM data of frame
	mad_synth_frame(&mad_synth, &mad_frame);
	output(&mad_synth.pcm, data->buffer, 1);

}
void carga_bff1(fsm_t* userData) {
	fsm_audio_controller_t* data = (fsm_audio_controller_t*) userData;
	flags_player&=~FLAG_BFF1_END;
	mad_frame_decode(&mad_frame, &mad_stream);
	// Synthesize PCM data of frame
	mad_synth_frame(&mad_synth, &mad_frame);
	output(&mad_synth.pcm, data->buffer, 0);
}



void Final_Melodia(fsm_t* userData) {
	printf("Final de la melodia \n");
	flags_player = 0;
	 Pa_StopStream( stream );
	 fclose(fp);

	 // Free MAD structs
	 mad_synth_finish(&mad_synth);
	 mad_frame_finish(&mad_frame);
	 mad_stream_finish(&mad_stream);
}



void output(struct mad_pcm *pcm, BUFFERS_T* buffer, uint8_t cbuff) {
	register int nsamples = pcm->length;
	mad_fixed_t const *left_ch = pcm->samples[0], *right_ch = pcm->samples[1];
	int i = 0;
	while (nsamples--) {
		signed int sample;
		sample = *left_ch++;
		if (cbuff == 0)
			*(buffer->buff1_l+i) = sample;
		else
			*(buffer->buff2_l+i) = sample;

		sample = *right_ch++;
		if (cbuff == 0)
			*(buffer->buff1_r+i) = sample;
		else
			*(buffer->buff2_r+i) = sample;

		i++;
	}

}


static int patestCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
	BUFFERS_T* buffer =(BUFFERS_T*) userData;
	int *out = (int*)outputBuffer;
	    unsigned long i;

	    (void) timeInfo; /* Prevent unused variable warnings. */
	    (void) statusFlags;
	    (void) inputBuffer;
	    if(buffer->currentBuffer == 1){
	    	buffer->currentBuffer = 0;
	    	lock(1);
	    	flags_player |= FLAG_BFF2_END;
	    	unlock(1);
	    }else{
	    	buffer->currentBuffer = 1;
	    	lock(1);
	    	flags_player |= FLAG_BFF1_END;
	    	unlock(1);
	    }
	    int j = 0;
	    for( i=0; i<framesPerBuffer; i++ )
	    {

	    	if(buffer->currentBuffer == 0){
	    		*out++ = *(buffer->buff1_l +i);  /* right */
	    		*out++ =*(buffer->buff1_r +i);
	    	}
			else{
				*out++ = *(buffer->buff2_l +i);  /* right */
				*out++=*(buffer->buff2_r +i);
			}
	    	j+=2;

	    }
	    buffer->sampleReaded++;

	    return paContinue;

}


BUFFERS_T* new_buffer(void){
	BUFFERS_T* new  = (BUFFERS_T*)malloc(sizeof(BUFFERS_T));
	new->buff1_l = (int*)malloc(sizeof(int)*FRAMES_PER_BUFFER);
	new->buff1_r = (int*)malloc(sizeof(int)*FRAMES_PER_BUFFER);
	new->buff2_l = (int*)malloc(sizeof(int)*FRAMES_PER_BUFFER);
	new->buff2_r = (int*)malloc(sizeof(int)*FRAMES_PER_BUFFER);
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

void launchPlayer(){
	pthread_attr_t tattr;
	pthread_t thread;
	fsm_audio_controller_t* sFsm = (fsm_audio_controller_t*)malloc(sizeof(fsm_audio_controller_t));
	sFsm->fsm = fsm_new(transition_table_polla,NULL);
	int newprio = 99;
	struct sched_param param;
	/* initialized with default attributes */
	pthread_attr_init (&tattr);

	/* safe to get existing scheduling param */
	pthread_attr_getschedparam (&tattr, &param);

	/* set the priority; others are unchanged */
	param.sched_priority = newprio;

	/* setting the new scheduling param */
	pthread_attr_setschedparam (&tattr, &param);
	pthread_create(&thread,&tattr, func, sFsm);

}
void func(void* data){
	fsm_audio_controller_t* sFsm = (fsm_audio_controller_t*)data;
	while (1) {
			fsm_fire(sFsm->fsm);
	}
}


