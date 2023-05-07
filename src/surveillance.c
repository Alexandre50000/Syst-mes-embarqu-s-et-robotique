#include <ch.h>
#include <hal.h>
#include <motors.h>

int VITESSE_ROTATION_SURVEILLANCE = 2 ;
bool detection;

// 1 cycle de rotation pour le mode surveillance
void rotation_gauche_droite(){
    // tourner 1/4 de tour à gauche
    left_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
    right_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
        // PAUSE 1s
    // tourner 1/4 de tour à droite
    left_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
    right_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
        // PAUSE 1s
    // tourner 1/4 de tour à gauche
    left_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
    right_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
        // PAUSE 1s
    // tourner 1/4 de tour à droite
    left_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
    right_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
        // PAUSE 1s
    // tourner 1/4 de tour à gauche
    left_motor_set_speed(-VITESSE_ROTATION_SURVEILLANCE);
    right_motor_set_speed(VITESSE_ROTATION_SURVEILLANCE);
}

// Mode surveillance
void surveillance(){
    while(!detection){
        rotation_gauche_droite();
        // si il y a une detection
        // alors detection = true
        // attaque()
    }

}

