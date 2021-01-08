/*
 * servo.c
 */

#include "servo.h"


// Initializes the correct ports and registers for moving the servo motor.

unsigned int pulse_period1 = (20 * 16000); // pulse period in cycles (20ms)*(16MHz)
//   0 degree = .85ms
//  90 degree = 1.55ms
// 180 degree = 2.25ms

 
#define REPLACE_ME 0x00



void servo_init1(void)
{
    unsigned int low_period = ((20 - 18.55) * 16000);  //match value
    // PORTB configuration
    SYSCTL_RCGCGPIO_R |= 0b0000010;              // enable clock GPIOB (page 340)
    GPIO_PORTB_AFSEL_R |= 0b00100000;             // sets PB5 as peripherals (page 671)
    GPIO_PORTB_PCTL_R |= 0x00700000;              // set PortB (PB5) to the alternate functions [T1CCP1] (page 688 & table 10.2)
    GPIO_PORTB_DEN_R |= 0b00100000;               // enable PB5
    GPIO_PORTB_DIR_R |= 0b00100000;               // set PB5 as an output

    //TIMER1B configuration
    SYSCTL_RCGCTIMER_R |= 0b000010;             // enable clock for TIMER 1 (page 338)
    TIMER1_CTL_R &= 0xFEFF;                   // disable the TimerB for Configuration (page 737)
    TIMER1_CFG_R |= 0x4;                   // select 16-bit timer configuration. (page 727)
    TIMER1_TBMR_R |= 0b000000001010;                  // set TIMER mode, PWM mode with periodic count (page 733)

    TIMER1_TBILR_R = (pulse_period1 & 0xFFFF);           // lower 16 bits of start value
    TIMER1_TBPR_R = (pulse_period1 >> 16);               // next 8 bits of the start value
 
    TIMER1_TBMATCHR_R = (int)(low_period & 0xFFFF);                 //set match value to 90 degree
    TIMER1_TBPMR_R = (int)(low_period >> 16);

    TIMER1_CTL_R |= 0x0100;                     // Enable TimerB for Configuration (page 737)
}

/**
Takes in an absolute degree value and moves the servo motor to that value if it is within 0 and 180 degrees.
        90
        ^
        |
  0 <---+---> 180
*/

void servo_move1(int degree)
{
//    unsigned int pulse_width;
//    float zeroDeg = 19.1;          //calibrated/machine
//    float ninetyDeg = 1.6;         //calibrated/machine
//    float oneEightyDeg = 17.7;        //calibrated/machine
//    float slope = 0;                //slope
//    float yInt = 0;                    //y - intercept (ms)
        unsigned int pulse_width;
        float zeroDeg = 18;          //calibrated/machine
        float ninetyDeg = 18.55;         //calibrated/machine
        float oneEightyDeg = 19.45;        //calibrated/machine
        float slope = 0;                //slope
        float yInt = 0;


    slope = ((zeroDeg - oneEightyDeg) / -180);      //calculates slope
    yInt = zeroDeg;                                 //sets y intercept

    pulse_width = (((slope * degree) + yInt) * 16000); //sets pulsewidth to slope times frequency
    TIMER1_TBMATCHR_R = (pulse_width & 0xFFFF);        //assigns pulsewidth to match register
    TIMER1_TBPMR_R = (pulse_width >> 16);
    timer_waitMillis(10);
}


