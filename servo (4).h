/*
 * servo.h
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "lcd.h"
#include "driverlib/interrupt.h"
#include <inc/tm4c123gh6pm.h>


void servo_init(void);
void servo_move(float degrees); 
void servo_init1(void);
void servo_move1(int degrees);

#endif /* SERVO_H_ */
