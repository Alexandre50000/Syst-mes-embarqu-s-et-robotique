#ifndef SURVEILLANCE_H
#define SURVEILLANCE_H

void rotation_180(uint8_t dir);
void rotation_surveillance(void);
void reset_motors(void);
void detection_init(void);
void attack(void);
uint8_t detection(void);
void wait_surv_exit(void);
#endif