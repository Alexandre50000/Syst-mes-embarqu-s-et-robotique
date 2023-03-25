#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <usbcfg.h>
#include <camera/po8030.h>

#include "process_image.h"
#include "main.h"


static float distance_cm = 0;


//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the lines USED_LINE and USED_LINE + 1 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 0, USED_LINE, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);
	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();
	systime_t time = 0;
    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done

		wait_image_ready();
		//signals an image has been captured
		chThdSleepMilliseconds(12);
		chBSemSignal(&image_ready_sem);  
    }
}


static THD_WORKING_AREA(waProcessImage, 1024);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff_ptr;
	uint8_t image[IMAGE_BUFFER_SIZE] = {0};

    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);
		//gets the pointer to the array filled with the last image in RGB565    
		img_buff_ptr = dcmi_get_last_image_ptr();
		
		/*
		*	To complete
		*/
		for(uint16_t i=0; i < 2*IMAGE_BUFFER_SIZE; i=i+2){
			image[i/2] = ((img_buff_ptr[i] & 0b00000111) << 3) | ((img_buff_ptr[i+1] & 0b11100000) >> 5);
		}
		
		chprintf((BaseSequentialStream *)&SD3, "Width= %d pixels \n \n",get_width(image));

		}

    }

uint16_t get_width(uint8_t* data){
	uint8_t average=0;
	uint16_t count=0;
	uint16_t width=0;

	for(uint16_t i=0; i < IMAGE_BUFFER_SIZE; ++i){
		count += data[i];
	}
	average = count/IMAGE_BUFFER_SIZE;

	for(uint16_t i=0; i < IMAGE_BUFFER_SIZE; ++i){
		if(data[i] < average){
			width += 1;
		}
	}
	return width;
}


float get_distance_cm(void){
	return distance_cm;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}