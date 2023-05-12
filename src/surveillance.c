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

// SStatic variable
static uint8_t detected = 0;

//semaphore
static BSEMAPHORE_DECL(detection_finished_sem, FALSE);
static BSEMAPHORE_DECL(Rotation_finished_sem, TRUE);

// Mode surveillance
static THD_WORKING_AREA(waSurveillance, 256);
static THD_FUNCTION(Surveillance, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        // Si son4 go control mode
        //if (get_value()==SOUND_4){
            //MODE CONTROL;
        //}
        // surveillance
        rotation_surveillance();
        
    }
}



void rotation_quart_gauche(void){
    if(detected){
        chBSemSignal(&Rotation_finished_sem);
        chBSemWait(&detection_finished_sem);
    }
    else{
        left_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
        right_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
    }
    uint8_t counter = 0; // Jusqu'à 36 pour faire un demi tour

    while(counter < 37 && !detected){
        chThdSleepMilliseconds(56);
        chBSemSignal(&Rotation_finished_sem);
        ++counter;
    }
}
void rotation_quart_droite(void){
    if(detected){
        chBSemSignal(&Rotation_finished_sem);
        chBSemWait(&detection_finished_sem);
    }
    else{
        left_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
        right_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
    }
    uint8_t counter = 0; // Jusqu'à 36 pour faire un demi tour

    while(counter < 37 && !detected){
        chThdSleepMilliseconds(56);
        chBSemSignal(&Rotation_finished_sem);
        ++counter;
    }
}

void rotation_surveillance(void){
    rotation_quart_gauche();
    reset_motors();
    chThdSleepSeconds(1); // PAUSE 1s
    rotation_quart_droite();
    reset_motors();
    chThdSleepSeconds(1); // PAUSE 1s
    rotation_quart_gauche();
    reset_motors();
    chThdSleepSeconds(1); // PAUSE
} 

void reset_motors(void){
    if(detected){
        chBSemSignal(&Rotation_finished_sem);
        chBSemWait(&detection_finished_sem);
    }
    left_motor_set_speed(0);
    right_motor_set_speed(0);
}

// detection
static THD_WORKING_AREA(waDetection, 256);
static THD_FUNCTION(Detection, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    

    while(1){
        detected = detection();
        if (detected){
            // Attends la fin d'une rotation
            chBSemWait(&Rotation_finished_sem);
            attaque();
            // detection passe à false
            detected = 0;
            chBSemSignal(&detection_finished_sem);

        }
        chThdSleepMilliseconds(20);
    }
}

void detection_init(void){
    chThdCreateStatic(waDetection, sizeof(waDetection), NORMALPRIO, Detection, NULL);
    chThdCreateStatic(waSurveillance, sizeof(waSurveillance), NORMALPRIO, Surveillance, NULL);
}

void attaque(void){
    // 3 slash en avancant
    left_motor_set_speed(VITESSE_ROTATION_ATTAQUE_INTERIEUR);
    right_motor_set_speed(VITESSE_ROTATION_ATTAQUE_EXTERIEUR);
    chThdSleepMilliseconds(500);
    left_motor_set_speed(0);
    right_motor_set_speed(0);
    chThdSleepMilliseconds(500);
    left_motor_set_speed(VITESSE_ROTATION_ATTAQUE_EXTERIEUR);
    right_motor_set_speed(VITESSE_ROTATION_ATTAQUE_INTERIEUR);
    chThdSleepMilliseconds(500);
    left_motor_set_speed(0);
    right_motor_set_speed(0);
    chThdSleepMilliseconds(500);
    left_motor_set_speed(VITESSE_ROTATION_ATTAQUE_INTERIEUR);
    right_motor_set_speed(VITESSE_ROTATION_ATTAQUE_EXTERIEUR);
    chThdSleepMilliseconds(500);
    left_motor_set_speed(0);
    right_motor_set_speed(0);
    chThdSleepMilliseconds(500);
    left_motor_set_speed(-VITESSE_ROTATION_ATTAQUE_EXTERIEUR);
    right_motor_set_speed(-VITESSE_ROTATION_ATTAQUE_EXTERIEUR);
    chThdSleepMilliseconds(1500);
    left_motor_set_speed(0);
    right_motor_set_speed(0);

}

int8_t detection(void){
    if (VL53L0X_get_dist_mm()<100){
        return 1;
    }
    else {
        return 0;
    }
}