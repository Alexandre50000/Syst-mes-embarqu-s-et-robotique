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
#include <camera/po8030.h>
#include <audio/audio_thread.h>



#include "main.h"
#include "audio_processing.h"
#include "image_processing.h"
#include "pickup.h"
#include "surveillance.h"
#include "vadrouille.h"
#include "control_robot.h"
#include "retour.h"

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

    // inits speaker
    dac_start();

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

    //inits camera
    dcmi_start();
	po8030_start();

    mic_start(&processAudioData);

    
    pickup_init();
    listen_init();
    process_image_start();
    retour_init();

    /* Infinite loop. */
    while (1) {
        // UNCOMMENT FOR FULL EXPERIENCE
        // switch (get_command())
        // {
        // case NOSOUND:
        //     break;
        // case SOUND_1:
        //     detection_init();
        //     wait_surv_exit();
        //     break;
        // case SOUND_2:
        //     vadrouille_init();
        //     wait_vadr_exit();
        //     break;
        // case SOUND_3:
        //     return_init();
        //     wait_rtrn_exit();
        //     break;
        // case SOUND_4:
        //     control_init();
        //     wait_control_exit();
        //     break;
        // }

        chThdSleepMilliseconds(50);
        }
}


#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void){
    chSysHalt("Stack smashing detected");
}


 