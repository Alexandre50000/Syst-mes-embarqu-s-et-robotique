#include <stdio.h>
#include <stdlib.h>
#include <ch.h>
#include <hal.h>
#include <motors.h>
#include <sensors/VL53L0X/VL53L0X.h>


#include "vadrouille.h"
#include "main.h"
#include "audio_processing.h"


// Définition des constantes
#define TURN_TIME   320201 // Durée de rotation en microsecondes
#define MAX_DIST_ONE_SEC 130 // en mm

//semaphore
static BSEMAPHORE_DECL(vadr_exit_sem, TRUE);

// Fonction de déplacement aléatoire
void randomMove(void) {

    uint16_t dist = VL53L0X_get_dist_mm();

    if(dist > MAX_DIST_ONE_SEC){
        dist = MAX_DIST_ONE_SEC;
    }

    // Si cible/mur est trop proche, ne pas avancer
    if(dist > 20){
    float time_advance = 1000*(float)(dist-20)/MAX_DIST_ONE_SEC; // We want at least two cm of distance between object

    // Appliquer les vitesses aux moteurs de mouvement
    left_motor_set_speed(MAX_SPEED);
    right_motor_set_speed(MAX_SPEED);

    chThdSleepMilliseconds((uint16_t)time_advance);
    }

    left_motor_set_speed(0);
    right_motor_set_speed(0);

    check_command(500);
    

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
    chThdSleepMicroseconds(TURN_TIME);

    // Arrêter les moteurs après la rotation
    left_motor_set_speed(0);
    right_motor_set_speed(0);

    check_command(500);
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
/*
* @brief  Checks command and waits
*
* @param Wait time in milliseconds (multiple of 50)
*/

void check_command(uint16_t waittime){
    uint16_t counter = waittime/50;
    while(counter != 0){
    if(get_command() == SOUND_5){
        chBSemSignal(&vadr_exit_sem);
        chThdExit(MSG_OK);
    }
    counter -= 1;
    chThdSleepMilliseconds(50);
    }

}


void vadrouille_init(void){
    chThdCreateStatic(waVadrouille, sizeof(waVadrouille), NORMALPRIO, Vadrouille, NULL);
}

void wait_vadr_exit(void){
    chBSemWait(&vadr_exit_sem);
}