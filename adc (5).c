/*
 * adc.c
 *
 *  Created on: Mar 5, 2019
 *      Author: Aidan
 */
#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "lcd.h"
#include "adc.h"
#include <inc/tm4c123gh6pm.h>
#define _REPLACEME 0x00

void adc_init(void)
{
    // section 13.4.1 in the datasheet  - page 817
    // Turn on clock for Port B
    SYSCTL_RCGCGPIO_R |= 0b000010; //Page 340
    SYSCTL_RCGCADC_R |= 0b01; // Enable ADC0 clock 352

    //Enable alternate function
    GPIO_PORTB_AFSEL_R |= 0b00010000; //set PB4 to alternate function page 650-671

    //Configure pins for input and analog mode
    GPIO_PORTB_DIR_R &= 0b11101111;     //Set PB4 to input mode 663
    GPIO_PORTB_DEN_R &= 0b11101111;    //Disable digital mode PB4 - 683
    GPIO_PORTB_AMSEL_R |= 0b00010000; //Enable analog mode PB4 page 687

    //Use default sample trigger source -- to be compatible with old versions of uC page 690
    GPIO_PORTB_ADCCTL_R = 0x00;

    //Configure ADC0 - Sample Seuqencer Configuration

    ADC0_ACTSS_R &= 0xFFFE; //Disable ADC0 SS0 by clearing bit 0 page 821

    // Set ADC SS0 functionality
    ADC0_EMUX_R &= 0xFFF0; // pocessor(default) page 837
    ADC0_SSMUX0_R |= 0xA; //Set 1st sample to user AIN10 ADC pin page 851

    //enable oversampling to average
    ADC0_SAC_R = 0x3;

    // Configure Sample Sequence control
    ADC0_SSCTL0_R |= 0x6; //page 853

    // Re-enable ADC SS0
    ADC0_ACTSS_R |= 0x1; //Re-enable ADC0 SS0 by setting bit 0

}
//used for polling
int adc_read(void)
{
    //starting up ADC conversion
    ADC0_PSSI_R |= 0x00000001;

    while ((ADC0_RIS_R & 1) == 0)
    {
        //waiting for conversion to be completed
    }

    //gives back raw ADC value
    ADC0_ISC_R = 0x01;
    return ADC0_SSFIFO0_R;

}
