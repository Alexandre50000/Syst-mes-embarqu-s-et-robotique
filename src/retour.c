#include <ch.h>
#include <hal.h>
#include <motors.h>
#include <sensors/VL53L0X/VL53L0X.h>

#include "main.h"
#include "image_processing.h"
#include "audio_processing.h"
#include "retour.h"

#define ROTATION_SPEED      160 // One quarter turn in 2s
#define MAX_DIST_ONE_SEC    130 // in mm

// Semaphore
static BSEMAPHORE_DECL(rtrn_exit_sem, TRUE);

/*
* Thread that searches for base and return to it.
*/
static THD_WORKING_AREA(waRetour, 256);
static THD_FUNCTION(Retour, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    
    right_motor_set_speed(ROTATION_SPEED);
    left_motor_set_speed(-ROTATION_SPEED);

    while(1){
        // Searches for base
        if(get_found()){
            move_to_base();
        }
        chThdSleepMilliseconds(50);
    }
}

void move_to_base(void){
    systime_t time;
    right_motor_set_speed(0);
    left_motor_set_speed(0);

    uint8_t middle = 0;
    do{
        time = chVTGetSystemTime();
        int16_t line_error = get_line_position()-IMAGE_BUFFER_SIZE/2;
        if(abs(line_error) < 10){
            right_motor_set_speed(0);
            left_motor_set_speed(0);
            middle = 1;
        }
        else{
            right_motor_set_speed(-line_error*ROTATION_COEFF);
            left_motor_set_speed(line_error*ROTATION_COEFF);


        }
        chThdSleepUntilWindowed(time, time + MS2ST(10)); // 100 Hz
    }while(!middle);

    uint16_t dist = (uint16_t)(VL53L0X_get_dist_mm());
    float time_advance = 1000*(float)(dist-20)/MAX_DIST_ONE_SEC; // We want at least two cm of distance between object

    left_motor_set_speed(MAX_SPEED);
    right_motor_set_speed(MAX_SPEED);

    chThdSleepMilliseconds((uint16_t)time_advance);

    // Turns around
    left_motor_set_speed(MAX_SPEED);
    right_motor_set_speed(-MAX_SPEED);

    chThdSleepMilliseconds(640); 

    
    left_motor_set_speed(0);
    right_motor_set_speed(0);
    // Signal to main that base was found
    chBSemSignal(&rtrn_exit_sem);
    chThdExit(MSG_OK);
}


void retour_init(void){
    select_color_detection(RED);
    chThdCreateStatic(waRetour, sizeof(waRetour), NORMALPRIO, Retour, NULL);
}

void wait_rtrn_exit(void){
    chBSemWait(&rtrn_exit_sem);
}

