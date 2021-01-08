/*
 *  movement.c
 *
 *  Created on: July 2, 2020
 *  Author: skimball
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <lcd.h>
#include <movement.h>
#include <open_interface.h>
#include <Timer.h>
#include <inc/tm4c123gh6pm.h>

void move(oi_t *sensor, int centimeters)
{
    //oi_t *sensor_data = oi_alloc();
    double sum = 0;

    if (centimeters > 0)
    {
        oi_setWheels(200, 200); // move forward
        while (sum < (centimeters * 10))
        {
            oi_update(sensor);
            sum += sensor->distance;
        }
    }
    else
    {
        oi_setWheels(-200, -200);   // move back
        while (sum > (centimeters * 10))
        {
            oi_update(sensor);
            sum += sensor->distance;
        }
    }
    oi_setWheels(0, 0); // stop
    //oi_free(sensor);
}

void turn(oi_t *sensor, int degrees)
{
    //oi_t *sensor_data = oi_alloc();
    double angle = 0;

    // Checks for CCW rotation
    if (degrees > 0)
    {
        oi_setWheels(50, -50);
        while (angle <= degrees)
        {
            oi_update(sensor);
            angle += (sensor->angle);
        }
        oi_setWheels(0, 0);
    }
    // Checks for CW rotation
    else if (degrees <= 0)
    {
        oi_setWheels(-50, 50);
        while (angle >= degrees)
        {
            oi_update(sensor);
            angle += (sensor->angle);
        }
        oi_setWheels(0, 0);
    }
    else
    {
        oi_setWheels(0, 0);
    }
    //oi_free(sensor);
}

void cliffEscLeft(oi_t *sensor_data, int distance)
{   //escape left
    move(sensor_data, -15);
    turn(sensor_data, 45);
    move(sensor_data, 45);
    distance = distance - 200; //accounts for the distance counted during lateral movement
    turn(sensor_data, -89);
    move(sensor_data, 45);
    turn(sensor_data, 45);
//    move(sensor_data, 25);
//    turn(sensor_data, 89);

    //distance += sensor_data->distance;
}

void cliffEscRight(oi_t *sensor_data, int distance)
{   //escape right
    move(sensor_data, -15);
    turn(sensor_data, -45);
    move(sensor_data, 45);
    distance = distance - 200; //accounts for the distance counted during lateral movement
    turn(sensor_data, 89);
    move(sensor_data, 45);
    turn(sensor_data, -45);
//    move(sensor_data, 25);
//    turn(sensor_data, -89);

    //distance += sensor_data->distance;
}

//void bumpEscLeft(oi_t *sensor_data, int distance)
//{   //escape left
//    move(sensor_data, -10);
//    turn(sensor_data, 89);
//    move(sensor_data, 25);
//    //distance = distance - 200; //accounts for the distance counted during lateral movement
//    turn(sensor_data, -89);
//    move(sensor_data, 20);
//    turn(sensor_data, -89);
//    move(sensor_data, 20);
//    turn(sensor_data, 89);
//
//    //distance += sensor_data->distance;
//}

//void bumpEscRight(oi_t *sensor_data, int distance)
//{   //escape right
//    move(sensor_data, -10);
//    turn(sensor_data, -89);
//    move(sensor_data, 20);
//    //distance = distance - 200; //accounts for the distance counted during lateral movement
//    turn(sensor_data, 89);
//    move(sensor_data, 20);
//    turn(sensor_data, 89);
//    move(sensor_data, 20);
//    turn(sensor_data, -89);
//
//}
