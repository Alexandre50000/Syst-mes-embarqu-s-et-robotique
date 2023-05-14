#include <ch.h>
#include <hal.h>
#include <motors.h>
#include <sensors/VL53L0X/VL53L0X.h>

#include "main.h"
#include "image_processing.h"
#include "audio_processing.h"
#include "retour.h"

#define ROTATION_SPEED              160 // Un quart de tour en 2 secondes
#define VITESSE_ROTATION_MIDDLE     178   // Vitesse pour faire 1° de deplacement en 20 ms
#define MAX_DIST_ONE_SEC 130 // en mm

// semaphore
static BSEMAPHORE_DECL(rtrn_exit_sem, TRUE);

// Mode retour à la base
static THD_WORKING_AREA(waRetour, 256);
static THD_FUNCTION(Retour, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;
    
    right_motor_set_speed(ROTATION_SPEED);
    left_motor_set_speed(-ROTATION_SPEED);

    while(1){
        // cherche base
        if(get_found()){
            deplacement_base();
        }
        chThdSleepMilliseconds(50);
    }
}

void deplacement_base(void){
    right_motor_set_speed(0);
    left_motor_set_speed(0);

    uint8_t middle = 0;
    do{
        uint16_t line_pos = get_line_position();
        if(line_pos < (IMAGE_BUFFER_SIZE/2-20)){
            right_motor_set_speed(VITESSE_ROTATION_MIDDLE);
            left_motor_set_speed(-VITESSE_ROTATION_MIDDLE);
        }
        else if(line_pos > (IMAGE_BUFFER_SIZE/2+20)){
            right_motor_set_speed(-VITESSE_ROTATION_MIDDLE);
            left_motor_set_speed(VITESSE_ROTATION_MIDDLE);
        }
        else{
            right_motor_set_speed(0);
            left_motor_set_speed(0);

            middle = 1;
        }
        chThdSleepMilliseconds(20);
    }while(!middle);

    uint16_t dist = (uint16_t)(get_distance_cm()*10);
    float time_advance = 1000*(float)(dist-20)/MAX_DIST_ONE_SEC; // We want at least two cm of distance between object

    left_motor_set_speed(MAX_SPEED);
    right_motor_set_speed(MAX_SPEED);

    chThdSleepMilliseconds((uint16_t)time_advance);

    // Fait un demi-tour
    left_motor_set_speed(MAX_SPEED);
    right_motor_set_speed(-MAX_SPEED);

    chThdSleepMilliseconds(640); 

    
    left_motor_set_speed(0);
    right_motor_set_speed(0);
    // Signal au mian que mode finit
    chBSemSignal(&rtrn_exit_sem);
    chThdExit(MSG_OK);
}


void retour_init(void){
    select_color_detection(GREEN);
    chThdCreateStatic(waRetour, sizeof(waRetour), NORMALPRIO, Retour, NULL);
}

void wait_rtrn_exit(void){
    chBSemWait(&rtrn_exit_sem);
}

