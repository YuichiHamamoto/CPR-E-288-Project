/*
 * ping.h
 *
 */

#ifndef PING_H_
#define PING_H_

#include "lcd.h"
#include "timer.h"
#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <math.h>

volatile enum {LOW, HIGH, DONE} State;

void ping_init();
void send_pulse();
float ping_read();
void TIMER3B_Handler(void);
 

#endif /* PING_H_ */
