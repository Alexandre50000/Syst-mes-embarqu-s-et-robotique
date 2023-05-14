#include <ch.h>
#include <hal.h>
#include <motors.h>

#include "audio_processing.h"
#include "control_robot.h"
#include "main.h"

// Sémaphores
static BSEMAPHORE_DECL(control_exit_sem, TRUE);

/*  
*   Thread that enables control of the robot
*/

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
            // Turns left
            left_motor_set_speed(-VITESSE_ROTATION_CONTROL);
            right_motor_set_speed(VITESSE_ROTATION_CONTROL);
            break;
        case SOUND_2:
            // Turns right
            left_motor_set_speed(VITESSE_ROTATION_CONTROL);
            right_motor_set_speed(-VITESSE_ROTATION_CONTROL);
            break;
        case SOUND_3:
            // Go foward
            left_motor_set_speed(VITESSE_AVANCE_CONTROL);
            right_motor_set_speed(VITESSE_AVANCE_CONTROL);
            break;
        case SOUND_4:
            // Go back
            left_motor_set_speed(-VITESSE_AVANCE_CONTROL);
            right_motor_set_speed(-VITESSE_AVANCE_CONTROL);
            break;
        case SOUND_5:
            //Returns to main
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