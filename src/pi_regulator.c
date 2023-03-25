#include <ch.h>
#include <hal.h>
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>

#include "pi_regulator.h"
#include "main.h"
#include "process_image.h"

#define NB_STEPS 1000.0f // for one full turn
#define WHEEL_P 13.0f  //cm


static THD_WORKING_AREA(waPiRegulator, 256);
static THD_FUNCTION(PiRegulator, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;

    int16_t speed = 0, speed_corr=0;
    float error=0, error_sum=0;
    float Kp=400, Ki=3;

    while(1){
        time = chVTGetSystemTime();
        error = get_distance_cm() - 10.0;
        error_sum += error;
        if(error_sum > MOTOR_SPEED_LIMIT){
            error_sum = MOTOR_SPEED_LIMIT;
        }
        else if(error_sum < -MOTOR_SPEED_LIMIT){
            error_sum = -MOTOR_SPEED_LIMIT;
        }
        speed = Kp*error+Ki*error_sum;
        speed_corr = (get_line_pos() - IMAGE_BUFFER_SIZE/2);
        
        right_motor_set_speed(speed- 2*speed_corr);
        left_motor_set_speed(speed+2*speed_corr);


        /*
		*	To complete
		*/
        
        //applies the speed from the PI regulator
		// right_motor_set_speed(speed);
		// left_motor_set_speed(speed);

        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void pi_regulator_start(void){
	chThdCreateStatic(waPiRegulator, sizeof(waPiRegulator), NORMALPRIO, PiRegulator, NULL);
}