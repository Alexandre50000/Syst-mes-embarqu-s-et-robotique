#include <ch.h>
#include <hal.h>
#include <motors.h>
#include <sensors/VL53L0X/VL53L0X.h>

#include "main.h"
#include "image_processing.h"
#include "audio_processing.h"
#include "surveillance.h"

#define VITESSE_ROTATION_SURVEILLANCE       321   // Un demi tour par 2 secondes
#define VITESSE_ROTATION_ATTAQUE_INTERIEUR  900
#define VITESSE_ROTATION_ATTAQUE_EXTERIEUR  900
#define VITESSE_ROTATION_MIDDLE             178   // Vitesse pour faire 1° de deplacement en 20 ms

// Static variables
static uint8_t detected = 0;
static uint8_t terminate = 0;

//semaphores
static BSEMAPHORE_DECL(detection_finished_sem, TRUE);
static BSEMAPHORE_DECL(rotation_finished_sem, TRUE);
static BSEMAPHORE_DECL(surv_exit_sem, TRUE);
// Mode surveillance
static THD_WORKING_AREA(waSurveillance, 256);
static THD_FUNCTION(Surveillance, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        // surveillance
        rotation_surveillance();
        
    }
}

void rotation_180(uint8_t dir){
    if(detected){
        chBSemSignal(&rotation_finished_sem);
        chBSemWait(&detection_finished_sem);
    }else if(terminate){        
        left_motor_set_speed(0);
        right_motor_set_speed(0);
        chThdExit(MSG_OK);
    }
    else{
        if(dir){
        left_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
        right_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
        }
        else{
        left_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
        right_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);}
    }
    uint8_t counter = 0; // Jusqu'à 36 pour faire un demi tour

    while(counter < 37 && !detected){
        chThdSleepMilliseconds(56);
        chBSemSignal(&rotation_finished_sem);
        ++counter;
    }
}

void rotation_surveillance(void){
    rotation_180(0);
    reset_motors();
    chThdSleepSeconds(1); // PAUSE 1s
    rotation_180(1);
    reset_motors();
    chThdSleepSeconds(1); // PAUSE 1s
    rotation_180(0);
    reset_motors();
    chThdSleepSeconds(1); // PAUSE
} 

void reset_motors(void){
    if(detected){
        chBSemSignal(&rotation_finished_sem);
        chBSemWait(&detection_finished_sem);
    }else if(terminate){        
        left_motor_set_speed(0);
        right_motor_set_speed(0);
        chThdExit(MSG_OK);
    }
    left_motor_set_speed(0);
    right_motor_set_speed(0);
}

// detection
static THD_WORKING_AREA(waDetection, 256);
static THD_FUNCTION(Detection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    thread_t* surv = chThdCreateStatic(waSurveillance, sizeof(waSurveillance), NORMALPRIO, Surveillance, NULL);

    while(!terminate){
        detected = detection();
        uint8_t com = get_command();
        if (detected){
            // Waits until end of rotation
            chBSemWait(&rotation_finished_sem);
            attack();
            // Resets detection.
            detected = 0;
            chBSemSignal(&detection_finished_sem);

        }
        else if(com == SOUND_5){
            terminate = 1;
            chThdWait(surv);
            chBSemSignal(&surv_exit_sem);

        }
        chThdSleepMilliseconds(20);
    }
}

void detection_init(void){
    terminate = 0;
    chThdCreateStatic(waDetection, sizeof(waDetection), NORMALPRIO, Detection, NULL);
}


/*
* @brief This function moves the robot to the middle of the target
*
*/
void middle(void){
    
    systime_t time;

    uint8_t middle = 0;
    do{
        time = chVTGetSystemTime();
        int16_t line_error = get_line_position()-IMAGE_BUFFER_SIZE/2;
        if(abs(line_error) < 70){
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
}

/*
* @brief Stwrts attack sequence
*/
void attack(void){
    // Rotates until target is in the middle of the camera
    middle();
    // thrusts foward to target
    uint16_t distance_mm = VL53L0X_get_dist_mm();
    if(distance_mm > DETECTION_DISTANCE){
        distance_mm = DETECTION_DISTANCE;
    }
    uint16_t steps = (uint16_t)(((float)distance_mm/130.0)*1000);

    left_motor_set_speed(steps);
    right_motor_set_speed(steps);
    chThdSleepMilliseconds(1000);
    left_motor_set_speed(-steps);
    right_motor_set_speed(-steps);
    chThdSleepMilliseconds(1000);
    left_motor_set_speed(0);
    right_motor_set_speed(0);

}

uint8_t detection(void){
    if (get_found() && (VL53L0X_get_dist_mm() < DETECTION_DISTANCE)){
        return 1;
    }
    else {
        return 0;
    }
}
void wait_surv_exit(void){
    chBSemWait(&surv_exit_sem);
}