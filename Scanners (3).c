/*
 * Scanners.c
 *
 *  Created on: Jul 7, 2020
 *      Author: bdunk
 */

/*****************************
 *
 * I just ripped this code from our lab 9 checkpoint 1. It all works and can be reused when we want
 * to scan for people while driving.
 *
 * The first and only function as of 07/07/20 is meant to scan at the end of the walk to find the home base
 * poles.
 *
 * All the functionality is the same as for checkpoint 1 until we get to the very end.
 *******************************/

#include <adc.h>
#include <ping.h>
#include <uart.h>
#include "timer.h"
#include "lcd.h"
#include "servo.h"
#include "math.h"
#include "stdio.h"

extern volatile float raw_adc_value;
extern volatile char uart_data;
extern volatile char flag;
extern volatile int cycle;

float end_scan()
{

    float IR;
    float cycle;
    float millis;
    float d1 = 0, d2 = 0;
    int flag = 0, calculate = 0;
    float dist;
    float theta = 1000.0;
    float width;
    float angle1, angle2, golden_angle;
    float smallest_t = 100.0;
    int objCount = 0;
    //int i = 0, j = 0;
    int i = 45, j = 0, count = 0;

    //while (i < 135)
    float theta_arr[20];
    for (i = 0; i < 180; i += 5)
    {
        servo_move1(i);
        //lcd_printf("%d", i);
        //reading and converting ping sensor
        cycle = ping_read();

        millis = (cycle / 16000.0);
        //t milliseconds * (1 sec/1000 milliseconds) * (340 meters/1 sec) * (100 cm/1 meter)
        // =
        // t * 34
        dist = (millis * 34) / 2.0;

        timer_waitMillis(150);
        //reading and converting IR sensor
        IR = adc_read();
        //raise the raw value to the power of x in the calibration graph
        IR = pow(IR, (-1.157)); //<-- this number changes depending on which computer you are on
        //divide the raw value by the slope of calibration graph
        IR = IR * (112211); //<-- this number changes depending on which computer you are on

        //debug print to the lcd to make sure we have all the right values
        // lcd_printf("PING: %0.2f cm\nIR: %0.2f\nAngle: %d", dist, IR, i);

        //if this printf function works, we then need to work out sending this data to putty
        char buff[100]; //<-- array to hold string version of distance value
        sprintf(buff, "PING: %0.2f cm\tIR: %0.2f \tAngle: %d \n", dist, IR, i);
        uart_SendStR(buff);

        // float theta_arr[20];
        float angle2_arr[200];
        //storing IR values in array for later use

        //angle2_arr[count] = IR;

        //finding the starting edge of the object
        if (IR < 250 && flag == 0)
        {
            angle1 = i;
            d1 = IR;
            flag = 1;

        }

        //finding the ending edge of the object
        else if (IR >= 250 && flag == 1)
        {

            angle2 = i - 1;
            //d2 = IR;
            //d2 = angle2_arr[count-1];
            flag = 2;
            calculate = 1;
            objCount++;
            timer_waitMillis(2000);
            lcd_printf("%d", objCount);

        }
        if (calculate == 1)
        {
            theta = angle2 - angle1;
            theta_arr[j] = angle1;
            calculate = 0;
            flag = 0;
            j++;
        }

        if (theta < smallest_t)
        {
            smallest_t = theta;
            golden_angle = angle1;
            lcd_printf("%f", golden_angle);
        }
        //i += 10;
        count += 1;

    }
    servo_move1(golden_angle);
    //converting the angle theta into radians for double angle calculation
    // smallest_t = ((smallest_t * M_PI) / 180);

    //width = (pow(d1, 2) + pow(d2, 2));
    //width = width - ((2 * d1 * d2) * cos(smallest_t));
    //width = sqrt(width);

    // lcd_printf("Width: %0.2f\nd1 = %0.2f\nd2 = %0.2f", width, d1, d2);

    //calculating the middle point between the two home base poles and returning the angle to turn the robot towards.
//        float a = theta_arr[1] - theta_arr[0];
//        lcd_printf("%f", a);
//        return a;
    return objCount;

}

int stop_scan()
{
    float IR;
    float theta = 1000.0;
    float angle1, angle2, golden_angle;
    int i = 0;
    int flag = 0;

    for (i = 0; i < 180; i += 5)
    {
        servo_move1(i);

        //reading and converting IR sensor
        IR = adc_read();
        //raise the raw value to the power of x in the calibration graph
        IR = pow(IR, (-1.153)); //<-- this number changes depending on which computer you are on
        //divide the raw value by the slope of calibration graph
        IR = IR * (108504); //<-- this number changes depending on which computer you are on

        // lcd_printf("PING: %0.2f cm\nIR: %0.2f\nAngle: %d", dist, IR, i);

        //if this printf function works, we then need to work out sending this data to putty
        char buff[100]; //<-- array to hold string version of distance value
        sprintf(buff, "IR: %0.2f \tAngle: %d \n", IR, i);
        uart_SendStR(buff);

        //finding the starting edge of the object
        if (IR < 250 && flag == 0)
        {
            angle1 = i;
            flag = 1;
        }

        //finding the ending edge of the object
        else if (IR >= 250 && flag == 1)
        {
            angle2 = i;
            flag = 0;
            break;
        }
    }

    return angle2;
}

int scan_detect()
{
    float info = 0;
    float millis = 0;
    float dist = 0;
    //send_pulse();
    info = ping_read();
    millis = (info / 16000.0);
    //t milliseconds * (1 sec/1000 milliseconds) * (340 meters/1 sec) * (100 cm/1 meter)
    // =
    // t * 34
    dist = (millis * 34) / 2.0;
    if (dist <= 30 && cycle >= 2)
    {
        oi_setWheels(0, 0);
        if (stop_scan() > 90)
        {
            return 1;

        }

        else if (stop_scan() <= 90)
        {
            return 0;

        }

    }
    return -1;
}

