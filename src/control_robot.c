#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <motors.h>

#include "audio_processing.h"
#include "control_robot.h"
#include "main.h"

// Sémaphores

static BSEMAPHORE_DECL(control_exit_sem, TRUE);


// Mode control
static THD_WORKING_AREA(waControl, 256);
static THD_FUNCTION(Control, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    uint8_t control_working = 1;

    while(control_working){
        switch (get_command())
        {
        case NOSOUND:
            left_motor_set_speed(0);
            right_motor_set_speed(0);
            break;
        case SOUND_1:
            // tourner à gauche
            left_motor_set_speed(-VITESSE_ROTATION_CONTROL);
            right_motor_set_speed(VITESSE_ROTATION_CONTROL);
            break;
        case SOUND_2:
            // tourner à droite
            left_motor_set_speed(VITESSE_ROTATION_CONTROL);
            right_motor_set_speed(-VITESSE_ROTATION_CONTROL);
            break;
        case SOUND_3:
            // avancer
            left_motor_set_speed(VITESSE_AVANCE_CONTROL);
            right_motor_set_speed(VITESSE_AVANCE_CONTROL);
            break;
        case SOUND_4:
            // reculer
            left_motor_set_speed(-VITESSE_AVANCE_CONTROL);
            right_motor_set_speed(-VITESSE_AVANCE_CONTROL);
            break;
        case SOUND_5:
            //quitter le mode controle
            control_working = 0;
            break;
        }
        chThdSleepMilliseconds(50);
    }
    
    left_motor_set_speed(0);
    right_motor_set_speed(0);
    chBSemSignal(&control_exit_sem);

}
void wait_control_exit(void){
    chBSemWait(&control_exit_sem);
}
void control_init(void){
    chThdCreateStatic(waControl, sizeof(waControl), NORMALPRIO, Control, NULL);
}