/*
 * ping.c
 *
 */

#define REPLACE_ME 0x00

#include "ping.h"

volatile unsigned int rising_time;  // start time of the return pulse
volatile unsigned int falling_time; // end time of the return pulse
volatile int overflow_counter = 0;

// Initializes GPIO PortB (PB3) and Timer3B (T3CCP1).
void ping_init()
{
    // initializes PORTB PB3:
    SYSCTL_RCGCGPIO_R |= 0b000010;            // enable clock GPIOB (page 340)
    GPIO_PORTB_AFSEL_R |= 0x08;           // sets PB3 as peripherals (page 671)
    GPIO_PORTB_DEN_R |= 0x08;             // enables PB3

    //initializes TimerB:
    SYSCTL_RCGCTIMER_R |= 0x08;            // Enable clock for timer3 (page 338)

    TIMER3_CTL_R &= 0xFEFF;   // Disable the TimerB for Configuration (page 737)
    TIMER3_CFG_R |= 0x4;        // Select 16-bit timer configuration. (page 727)
    TIMER3_TBMR_R |= 0b00010111; // Select TimerB mode: input capture mode, Edge-Time mode, Count up (page 733)

    TIMER3_TBPR_R |= 0xFF;                //Load initial max value (page 761)
    TIMER3_TBILR_R |= 0xEFFF;               //Load initial max value  (page 757)

    TIMER3_CTL_R |= 0x0C00;     //Set both edges to trigger interrupt (page 737)

    //clear TIMER3B interrupt flags
    TIMER3_ICR_R |= 0x0400; // Clear TimerB Capture Mode Event Interrupt Clear (page 754)
    TIMER3_IMR_R |= 0x0400; // Enable TimerB Capture Mode Event Interrupt Mask (page 745)

    //initialize local interrupts
    //NVIC_PRI9_R |= 0x0020;                   //Sets bits 7:5 to 0b001
    NVIC_EN1_R |= 0x10;                 //Enable interrupt for T3CCP1
    IntRegister(INT_TIMER3B, TIMER3B_Handler);   //Link interrupt to handler
    //TIMER3_CTL_R |= 0x0100;

}

// Sends out a pulse with the sonar sensor on PB3.
void send_pulse()
{
    TIMER3_CTL_R &= 0xFEFF;       // Disable TimerB for Configuration (page 737)

    GPIO_PORTB_AFSEL_R &= 0xF7;    // change PortB (PB3) to GPIO mode (page 671)
    GPIO_PORTB_DIR_R |= 0x08;            // set PortB (PB3) as output (page 663)

    GPIO_PORTB_DATA_R |= 0x08;              // set PB3 to high (page 662)
    timer_waitMicros(5);                          // wait 5 us
    GPIO_PORTB_DATA_R &= 0xF7;               // set PB3 to low  (page 662)

    GPIO_PORTB_DIR_R &= 0xF7;             // set PortB (PB3) as input (page 663)
    GPIO_PORTB_AFSEL_R |= 0x08; // change PortB (PB3) to alternate function mode (page 671)

    GPIO_PORTB_PCTL_R |= 0x00007000; // Set PortB (PB3) to the alternate functions [T3CCP1] (page 688 & table 10.2)
    TIMER3_CTL_R |= 0x0100;        // Enable TimerB for Configuration (page 737)
    State = LOW;
}

//  Sends a pulse out with the sensor and returns the distance in cm.
float ping_read()
{
    float cyc;

    send_pulse();
   // while (State != DONE)
    //{
    //}
    //State = LOW;
    cyc = falling_time - rising_time;

    //accounting for timer overflow
    //(we shouldn't have any issue with this because we are counting up not down?)
    if (cyc < 0)
    {
        cyc = abs(cyc);
        overflow_counter++;
    }
    return cyc;
}

// ping sensor interrupt handler
void TIMER3B_Handler(void)
{
    //state values change in interrupt handler
    if (State == DONE)
    {
        State = LOW;
    }
    if (State == LOW)
    {
        rising_time = TIMER3_TBR_R;
        State = HIGH;
    }

    else if (State == HIGH)
    {
        falling_time = TIMER3_TBR_R;
        State = DONE;
    }
    TIMER3_ICR_R |= 0x0400;
}

