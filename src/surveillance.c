#include <ch.h>
#include <hal.h>
#include <motors.h>

#include "image_processing.h"

#define VITESSE_ROTATION_SURVEILLANCE   0.8
#define VITESSE_ROTATION_ATTAQUE_INTERIEUR  3.4
#define VITESSE_ROTATION_ATTAQUE_EXTERIEUR  3.8

//semaphore
static BSEMAPHORE_DECL(detection_sem, FALSE);

// Mode surveillance
static THD_WORKING_AREA(waSurveillance, 256);
static THD_FUNCTION(Surveillance, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        // tant qu'une detection n'est pas faite
        chBSemWait(&detection_sem);
        // surveillance
        rotation_surveillance();
    }
}

void surveillance_init(void){
    chThdCreateStatic(waSurveillance, sizeof(waSurveillance), NORMALPRIO, Surveillance, NULL);
}

void rotation_quart_gauche(){
    left_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
    right_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
}

void rotation_quart_droite(){
    left_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
    right_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
}

void    rotation_surveillance(){
    rotation_quart_gauche();
    chThdSleepS(4); // temps pour faire 1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
    rotation_quart_droite();
    chThdSleepS(4); // temps pour faire 1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
    rotation_quart_gauche();
    chThdSleepS(4); // temps pour faire 1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
    rotation_quart_droite();
    chThdSleepS(4); // temps pour faire 1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
    rotation_quart_gauche();
    chThdSleepS(4); // temps pour faire 1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
} 

void reset_motors(){
    left_motor_set_speed(0);
    right_motor_set_speed(0);
}

// detection
static THD_WORKING_AREA(waAttaque, 256);
static THD_FUNCTION(Attaque, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        if (detection()){
            //signale qu'une détection a été faite
		    chBSemSignal(&detection_sem);
            attaque();
            // retour à la base grâce au son
        }
    }
}

void attaque_init(void){
    chThdCreateStatic(waAttaque, sizeof(waAttaque), NORMALPRIO, Attaque, NULL);
}

void attaque(){
    // 3 slash en avancant
    left_motor_set_speed(VITESSE_ROTATION_ATTAQUE_INTERIEUR);
    right_motor_set_speed(VITESSE_ROTATION_ATTAQUE_EXTERIEUR);
    // pause d'un certain temps
    left_motor_set_speed(VITESSE_ROTATION_ATTAQUE_EXTERIEUR);
    right_motor_set_speed(VITESSE_ROTATION_ATTAQUE_INTERIEUR);
    // pause d'un certain temps
    left_motor_set_speed(VITESSE_ROTATION_ATTAQUE_INTERIEUR);
    right_motor_set_speed(VITESSE_ROTATION_ATTAQUE_EXTERIEUR);
}

bool detection(){
    if (get_distance_cm()<10){
        return true;
    }
    else {
        return false;
    }
}