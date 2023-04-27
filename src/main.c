#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ch.h>
#include <hal.h>
#include <memory_protection.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>
#include <arm_math.h>
#include <sensors/VL53L0X/VL53L0X.h>

#include "main.h"
#include "audio_processing.h"
#include "fft.h"
#include "communications.h"

//uncomment to send the FFTs results from the real microphones
//#define SEND_FROM_MIC

//uncomment to use double buffering to send the FFT to the computer
#define DOUBLE_BUFFERING

static void serial_start(void){
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

int main(void){

    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();
    //starts the USB communication
    usb_start();

    //inits the motors
    motors_init();

    //inits the ToF captor
    VL53L0X_start();

    /* Infinite loop. */
    while (1) {

        uint16_t distance;
        distance = VL53L0X_get_dist_mm();
        chprintf((BaseSequentialStream *) &SD3, "Distance = %d \n", distance);
        chThdSleepMilliseconds(20);
        }
}


#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
    chSysHalt("Stack smashing detected");
}
