#include <ch.h>
#include <hal.h>
#include <motors.h>

#define VITESSE_ROTATION_SURVEILLANCE   2

// Mode surveillance
static THD_WORKING_AREA(waSurveillance, 256);
static THD_FUNCTION(Surveillance, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
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
    // SLEEP T=1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
    rotation_quart_droite();
    // SLEEP T=1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
    rotation_quart_gauche();
    // SLEEP T=1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
    rotation_quart_droite();
    // SLEEP T=1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
    rotation_quart_gauche();
    // SLEEP T=1/4 de tour
    reset_motors();
    chThdSleepS(1); // PAUSE 1s
} 

void reset_motors(){
    left_motor_set_speed(0);
    right_motor_set_speed(0);
}

// Attaque après détection
static THD_WORKING_AREA(waAttaque, 256);
static THD_FUNCTION(Attaque, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        attaque();
    }
}

void attaque_init(void){
    chThdCreateStatic(waAttaque, sizeof(waAttaque), NORMALPRIO, Attaque, NULL);
}

void attaque(){
    // 3 slash en avancant
}