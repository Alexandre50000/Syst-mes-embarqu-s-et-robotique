#include <ch.h>
#include <hal.h>
#include <motors.h>

#include "audio_processing.h"
#include "control_robot.h"

// Mode control
static THD_WORKING_AREA(waControl, 256);
static THD_FUNCTION(Control, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        if (get_value() == SOUND_1){
            // tourner à gauche
            left_motor_set_speed(-VITESSE_ROTATION_CONTROL);
            right_motor_set_speed(VITESSE_ROTATION_CONTROL);
        }
        if (get_value() == SOUND_2){
            // tourner à droite
            left_motor_set_speed(VITESSE_ROTATION_CONTROL);
            right_motor_set_speed(-VITESSE_ROTATION_CONTROL);
        }
        if (get_value() == SOUND_3){
            left_motor_set_speed(VITESSE_AVANCE_CONTROL);
            right_motor_set_speed(VITESSE_AVANCE_CONTROL);
        }
        if (get_value() == SOUND_4){
            // MODE SURVEILLANCE
        }
    }
}

void control_init(){
    chThdCreateStatic(waControl, sizeof(waControl), NORMALPRIO, Control, NULL);
}