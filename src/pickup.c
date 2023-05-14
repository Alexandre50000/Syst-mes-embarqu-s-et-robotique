#include <ch.h>
#include <hal.h>
#include <sensors/proximity.h>
#include <leds.h>
#include <sensors/imu.h>
#include <audio/audio_thread.h>




#include "pickup.h"
#include "main.h"

#define MIN_DISTANCE    100
#define MIN_SENSORS     4    //Minimum amount of sensors that need to trigger befor panic
#define THRESHOLD       4000 //Value of IMU when acc is equal to around 0.25g        

/*
* Thread that detects pickup then sounds speaker and activates leds
*/
static THD_WORKING_AREA(waPickup, 256);
static THD_FUNCTION(Pickup, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    while(1){
        if(check_pickup()){
        start_led_sequence();}
        chThdSleepMilliseconds(100);
    }



}

void start_led_sequence(void){
    set_led(NUM_LED, 1);
    dac_play(440);
    chThdSleepMilliseconds(100);
    set_led(NUM_LED,0);
    set_rgb_led(LED2,0,255,0);
    set_rgb_led(LED4,0,255,0);
    set_rgb_led(LED6,0,255,0);
    set_rgb_led(LED8,0,255,0);
    dac_play(400);
    chThdSleepMilliseconds(100);
    clear_leds();
    dac_stop();
}

void pickup_init(void){
    chThdCreateStatic(waPickup, sizeof(waPickup), NORMALPRIO, Pickup, NULL);
}


/*
* @brief This function checks for pickup
* @returns 1 if it detects pickup, 0 otherwise
*/
int8_t check_pickup(void){
    int8_t triggers;
    triggers = 0;
    int16_t acc_x;
    int16_t acc_y;

    acc_x = abs(get_acc(0));
    acc_y = abs(get_acc(1));

    for(int8_t i=0; i < 8; ++i){
        if(get_prox(i) > MIN_DISTANCE){
            ++triggers;
        }
    }
    if((triggers >= MIN_SENSORS) && (acc_x > THRESHOLD || acc_y > THRESHOLD)){
        return 1;
    }
    else{
        return 0;
    }
}

