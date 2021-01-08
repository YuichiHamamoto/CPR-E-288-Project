/*
 * adc.h
 *
 *  Created on: Mar 5, 2019
 *      Author: Aidan
 */

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "lcd.h"
#include <inc/tm4c123gh6pm.h>
#ifndef ADC_H_
#define ADC_H_
// Initialize ADC
void adc_init(void);
// read distance with ADC
int adc_read(void);

#endif /* ADC_H_ */
