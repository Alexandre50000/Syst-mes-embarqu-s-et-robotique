#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <usbcfg.h>
#include <camera/po8030.h>

#include "process_image.h"
#include "main.h"


static float distance_cm = 0;
static uint16_t line_pos = IMAGE_BUFFER_SIZE/2;


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

		uint16_t linewidth=0;
		linewidth = get_width(image);
		distance_cm = CONV_PX_CM/(float)linewidth;


		
		chprintf((BaseSequentialStream *)&SD3, "Distance= %2f cm  Pixel Width= %d \n",distance_cm, linewidth);

		}

    }

uint16_t get_width(uint8_t* data){
	uint8_t average=0;
	uint16_t count=0, width=0, begin=0, end=0, i=0;
	bool stop=0, line_not_found=0, wrong_line=0;
	static uint16_t last_width = CONV_PX_CM/GOAL_DISTANCE;

	for(uint16_t i=0; i < IMAGE_BUFFER_SIZE; ++i){
		count += data[i];
	}
	average = count/IMAGE_BUFFER_SIZE;

	do{
		wrong_line = 0;
		while(stop==0 && i < (IMAGE_BUFFER_SIZE-SLOPE_WIDTH)){
			if(data[i] > average &&  data[i+SLOPE_WIDTH] < average){
				begin = i;
				stop = 1;
			}
			++i;
		}
		if(i <(IMAGE_BUFFER_SIZE-SLOPE_WIDTH) && begin){
			stop = 0;
			while(stop==0 && i < (IMAGE_BUFFER_SIZE-SLOPE_WIDTH)){
				if(data[i]< average && data[i+SLOPE_WIDTH] > average){
					stop=1;
					end=i;
				}
				++i;

			}
			if(i>IMAGE_BUFFER_SIZE || end ==0){
			line_not_found=1;
			}

		}
		else{
			line_not_found=1;
		}
		
		if(!line_not_found && (end-begin)<MIN_LINE_WIDTH){
			i = end;
			begin=0;
			end=0;
			stop=0;
			wrong_line= 1;
			}
	}while(wrong_line);

	if(line_not_found) {
		begin = 0;
		end = 0;
		width = last_width;
	}
	else{
		last_width = width = (end-begin);
		line_pos = (end+begin)/2;
	}
	return width;
}


float get_distance_cm(){
	return distance_cm;
}

uint16_t get_line_pos(){
	return line_pos;
}

void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}