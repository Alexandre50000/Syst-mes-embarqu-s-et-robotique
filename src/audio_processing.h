#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H


#define FFT_SIZE 	1024

#define MIN_VALUE_THRESHOLD	10000

#define MIN_FREQ		30	//we don't analyze before this index to not use resources for nothing
#define MAX_FREQ		50	//we don't analyze after this index to not use resources for nothing

#define SOUND_1_L		(SOUND_1-1)
#define SOUND_1_H		(SOUND_1+1)
#define SOUND_2_L		(SOUND_2-1)
#define SOUND_2_H		(SOUND_2+1)
#define SOUND_3_L		(SOUND_3-1)
#define SOUND_3_H		(SOUND_3+1)
#define SOUND_4_L		(SOUND_4-1)
#define SOUND_4_H		(SOUND_4+1)
#define SOUND_5_L		(SOUND_5-1)
#define SOUND_5_H		(SOUND_5+1)

typedef enum {
	//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
	LEFT_CMPLX_INPUT = 0,
	RIGHT_CMPLX_INPUT,
	FRONT_CMPLX_INPUT,
	BACK_CMPLX_INPUT,
	//Arrays containing the computed magnitude of the complex numbers
	LEFT_OUTPUT,
	RIGHT_OUTPUT,
	FRONT_OUTPUT,
	BACK_OUTPUT
} BUFFER_NAME_t;



typedef struct complex_float{
	float real;
	float imag;
}complex_float;

void doFFT_optimized(uint16_t size, float* complex_buffer);

void processAudioData(int16_t *data, uint16_t num_samples);

/*
*	put the invoking thread into sleep until it can process the audio datas
*/
void wait_for_data(void);
int16_t get_command(void);
void compute_command(void);

void listen_init(void);
/*
*	Returns the pointer to the BUFFER_NAME_t buffer asked
*/
float* get_audio_buffer_ptr(BUFFER_NAME_t name);

#endif /* AUDIO_PROCESSING_H */