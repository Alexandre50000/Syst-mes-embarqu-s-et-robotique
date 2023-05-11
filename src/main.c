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
#include <sensors/proximity.h>
#include <leds.h>
#include <spi_comm.h>
#include <sensors/imu.h>


#include "main.h"
#include "audio_processing.h"
#include "fft.h"
#include "communications.h"
#include "pickup.h"
#include "surveillance.h"

//uncomment to send the FFTs results from the real microphones
//#define SEND_FROM_MIC

//uncomment to use double buffering to send the FFT to the computer
#define DOUBLE_BUFFERING

// declares bus to start bus communication for optical sensors
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

parameter_namespace_t parameter_root, aseba_ns;

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

    /** Inits the Inter Process Communication bus. */
    messagebus_init(&bus, &bus_lock, &bus_condvar);

    //starts the serial communication
    serial_start();

    //starts SPI communication for RGB LEDS
    spi_comm_start();

    //starts the USB communication
    usb_start();

    //inits the motors
    motors_init();

    //inits the ToF captor
    VL53L0X_start();

    //inits the proximity sensors
    proximity_start();
    calibrate_ir();

    //inits the IMU
    imu_start();
    calibrate_acc();

    //pickup_init();

    mic_start(&processAudioData);

    detection_init();


    /* Infinite loop. */
    while (1) {
        
        chThdSleepMilliseconds(100);
        }
}


#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
    chSysHalt("Stack smashing detected");
}
