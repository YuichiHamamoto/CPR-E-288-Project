/*
 *  movement.h
 *
 *  Created on: July 2, 2020
 *  Author: skimball
 *
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <lcd.h>
#include <open_interface.h>
#include <Timer.h>
#include <inc/tm4c123gh6pm.h>

void move(oi_t *sensor, int speed);
void turn(oi_t *sensor, int degrees);
void bumpEscRight(oi_t *sensor, int speed);
void bumpEscLeft(oi_t *sensor, int speed);
void cliffEscRight(oi_t *sensor, int speed);
void cliffEscFrontRight(oi_t *sensor, int speed);
void cliffEscLeft(oi_t *sensor, int speed);
void cliffEscFrontLeft(oi_t *sensor, int speed);
#endif /* MOVEMENT_H_ */
