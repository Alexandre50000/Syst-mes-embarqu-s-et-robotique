#include <stdio.h>
#include <stdlib.h>
#include <ch.h>
#include <hal.h>
#include <motors.h>


#include "vadrouille.h"
#include "main.h"


// Définition des constantes
#define MAX_SPEED   1000 // [step/s]
#define TURN_TIME   1000 // Durée de rotation en millisecondes

// Fonction de déplacement aléatoire
void randomMove(void) {
    int leftSpeed, rightSpeed;
  
    // Génération de vitesses aléatoires pour les moteurs
    leftSpeed = rand() % (2 * MAX_SPEED) - MAX_SPEED;
    rightSpeed = rand() % (2 * MAX_SPEED) - MAX_SPEED;

    // Appliquer les vitesses aux moteurs de mouvement
    left_motor_set_speed(leftSpeed);
    right_motor_set_speed(rightSpeed);

    // Attendre une durée aléatoire pour la rotation
    int turnTime = rand() % TURN_TIME;
    chThdSleepMilliseconds(turnTime);

    // Arrêter les moteurs après la rotation
    left_motor_set_speed(0);
    right_motor_set_speed(0);
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