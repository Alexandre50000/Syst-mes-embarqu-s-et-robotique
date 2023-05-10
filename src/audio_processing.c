#include <ch.h>
#include <hal.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>
#include <arm_math.h>

#include "audio_processing.h"
#include "main.h"
#include "fft.h"
#include "communications.h"

//semaphore
static BSEMAPHORE_DECL(micDataReady_sem, TRUE);
static BSEMAPHORE_DECL(commandReady, TRUE);

//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];
static float micRight_cmplx_input[2 * FFT_SIZE];
static float micFront_cmplx_input[2 * FFT_SIZE];
static float micBack_cmplx_input[2 * FFT_SIZE];
//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];
static float micRight_output[FFT_SIZE];
static float micFront_output[FFT_SIZE];
static float micBack_output[FFT_SIZE];

// Static variables containing the frequency of command
static int16_t hertz;


#define MIN_VALUE_THRESHOLD	6000 

#define MIN_FREQ		30	//we don't analyze before this index to not use resources for nothing
#define SOUND_1			32	//500Hz For base recognition
#define SOUND_2			36	//Around 560Hz Patrol Attack
#define SOUND_3			39	//Around 610HZ Patrol Defense
#define SOUND_4			42	//Around 650Hz Controlled Patrol
#define NOSOUND         0   // When no new command was perceived
#define MAX_FREQ		45	//we don't analyze after this index to not use resources for nothing

#define SOUND_1_L		(SOUND_1-1)
#define SOUND_1_H		(SOUND_1+1)
#define SOUND_2_L		(SOUND_2-1)
#define SOUND_2_H		(SOUND_2+1)
#define SOUND_3_L		(SOUND_3-1)
#define SOUND_3_H		(SOUND_3+1)
#define SOUND_4_L		(SOUND_4-1)
#define SOUND_4_H		(SOUND_4+1)
#define NORM			(16000/1024)


/*  
*   Thread that listens for command
*	It is always active and must transmit
*	data to main after hearing a sound
*/

static THD_WORKING_AREA(waListen, 256);
static THD_FUNCTION(Listen, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
		wait_for_data();
		compute_command();
		chThdSleepMilliseconds(50);
    }
}


void compute_command(void){

	int8_t freq;
	uint16_t max_norm;
	freq = -1;
	max_norm = MIN_VALUE_THRESHOLD;

	for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++){
		uint16_t average = 0;
		average += micFront_output[i];
		average += micBack_output[i];
		average += micLeft_output[i];
		average += micRight_output[i];
		average /= 4;
		if(average > max_norm){
			max_norm = average;
			freq = i;
		}
	}

	if(freq >= SOUND_1_L && freq<= SOUND_1_H){
		hertz = SOUND_1;
	}
	
	else if(freq >= SOUND_2_L && freq <= SOUND_2_H){
		hertz = SOUND_2;
	}
	else if(freq >= SOUND_3_L && freq <= SOUND_3_H){
		hertz = SOUND_3;
	}
	else if(freq >= SOUND_4_L && freq <= SOUND_4_H){
		hertz = SOUND_4;
	}
	else{
		hertz = NOSOUND;
	}

	chBSemSignal(&commandReady);
	
}

/*
*	Callback called when the demodulation of the four microphones is done.
*	We get 160 samples per mic every 10ms (16kHz)
*	
*	params :
*	int16_t *data			Buffer containing 4 times 160 samples. the samples are sorted by micro
*							so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
*	uint16_t num_samples	Tells how many data we get in total (should always be 640)
*/
void processAudioData(int16_t *data, uint16_t num_samples){

	/*
	*
	*	We get 160 samples per mic every 10ms
	*	So we fill the samples buffers to reach
	*	1024 samples, then we compute the FFTs.
	*
	*/

	static uint16_t nb_samples = 0;

	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4){
		//construct an array of complex numbers. Put 0 to the imaginary part
		micRight_cmplx_input[nb_samples] = (float)data[i + MIC_RIGHT];
		micLeft_cmplx_input[nb_samples] = (float)data[i + MIC_LEFT];
		micBack_cmplx_input[nb_samples] = (float)data[i + MIC_BACK];
		micFront_cmplx_input[nb_samples] = (float)data[i + MIC_FRONT];

		nb_samples++;

		micRight_cmplx_input[nb_samples] = 0;
		micLeft_cmplx_input[nb_samples] = 0;
		micBack_cmplx_input[nb_samples] = 0;
		micFront_cmplx_input[nb_samples] = 0;

		nb_samples++;

		//stop when buffer is full
		if(nb_samples >= (2 * FFT_SIZE)){
			break;
		}
	}

	if(nb_samples >= (2 * FFT_SIZE)){
		/*	FFT proccessing
		*
		*	This FFT function stores the results in the input buffer given.
		*	This is an "In Place" function. 
		*/

		doFFT_optimized(FFT_SIZE, micRight_cmplx_input);
		doFFT_optimized(FFT_SIZE, micLeft_cmplx_input);
		doFFT_optimized(FFT_SIZE, micFront_cmplx_input);
		doFFT_optimized(FFT_SIZE, micBack_cmplx_input);

		/*	Magnitude processing
		*
		*	Computes the magnitude of the complex numbers and
		*	stores them in a buffer of FFT_SIZE because it only contains
		*	real numbers.
		*
		*/
		arm_cmplx_mag_f32(micRight_cmplx_input, micRight_output, FFT_SIZE);
		arm_cmplx_mag_f32(micLeft_cmplx_input, micLeft_output, FFT_SIZE);
		arm_cmplx_mag_f32(micFront_cmplx_input, micFront_output, FFT_SIZE);
		arm_cmplx_mag_f32(micBack_cmplx_input, micBack_output, FFT_SIZE);

		nb_samples = 0;
		chBSemSignal(&micDataReady_sem);
		}
}


void wait_for_data(void){
	chBSemWait(&micDataReady_sem);
}


void listen_init(void){
	chThdCreateStatic(waListen, sizeof(waListen), NORMALPRIO, Listen, NULL);
}

int16_t get_command(void){
	chBSemWait(&commandReady);
	return hertz;
}


float* get_audio_buffer_ptr(BUFFER_NAME_t name){
	if(name == LEFT_CMPLX_INPUT){
		return micLeft_cmplx_input;
	}
	else if (name == RIGHT_CMPLX_INPUT){
		return micRight_cmplx_input;
	}
	else if (name == FRONT_CMPLX_INPUT){
		return micFront_cmplx_input;
	}
	else if (name == BACK_CMPLX_INPUT){
		return micBack_cmplx_input;
	}
	else if (name == LEFT_OUTPUT){
		return micLeft_output;
	}
	else if (name == RIGHT_OUTPUT){
		return micRight_output;
	}
	else if (name == FRONT_OUTPUT){
		return micFront_output;
	}
	else if (name == BACK_OUTPUT){
		return micBack_output;
	}
	else{
		return NULL;
	}
}

