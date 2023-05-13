#include <stdio.h>
#include <stdlib.h>
#include <ch.h>
#include <hal.h>
#include <motors.h>
#include <sensors/VL53L0X/VL53L0X.h>


#include "vadrouille.h"
#include "main.h"


// Définition des constantes
#define MAX_SPEED   1000 // [step/s]
#define TURN_TIME   1000 // Durée de rotation en millisecondes

// Fonction de déplacement aléatoire
void randomMove(void) {

    // Appliquer les vitesses aux moteurs de mouvement
    left_motor_set_speed(MAX_SPEED);
    right_motor_set_speed(MAX_SPEED);
    
    chThdSleepMilliseconds(500);

    left_motor_set_speed(0);
    right_motor_set_speed(0);

    chThdSleepMilliseconds(500);
    

    // Attendre une direction aléatoire pour la rotation
    int8_t turndir = rand() % 2;
    if(turndir){
        left_motor_set_speed(-MAX_SPEED);
        right_motor_set_speed(MAX_SPEED);

    }
    else{
        left_motor_set_speed(MAX_SPEED);
        right_motor_set_speed(-MAX_SPEED);

    }
    chThdSleepMicroseconds(320200);

    // Arrêter les moteurs après la rotation
    left_motor_set_speed(0);
    right_motor_set_speed(0);
    
    chThdSleepMilliseconds(500);
}


static THD_WORKING_AREA(waVadrouille, 256);
static THD_FUNCTION(Vadrouille, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    // Initialiser le générateur de nombres aléatoires
    srand(chVTGetSystemTime());

    while(1){
        // Appeler la fonction de déplacement aléatoire
        randomMove();

        // Attendre un certain temps avant le prochain déplacement
        chThdSleepMilliseconds(500);
    }


}


void vadrouille_init(void){
    chThdCreateStatic(waVadrouille, sizeof(waVadrouille), NORMALPRIO, Vadrouille, NULL);
}