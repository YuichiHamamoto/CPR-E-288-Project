/*
 *
 * project_template.c
 *
 */

#define _RESET 0
#define _PART1 1
#include "timer.h"
#include "lcd.h"
#include <adc.h>
#include <ping.h>
#include "servo.h"
#include <uart.h>
#include <movement.h>
#include "math.h"
#include "stdio.h"
#include "button.h"
#include "Scanners.h"

volatile int cycle;
#if _RESET
#include "Simulation_User Files_resetSimulation.h"
#endif

void main()
{
#if _RESET
    resetSimulationBoard();
#else

    timer_init();
    lcd_init();
    button_init();
    init_button_interrupts();
    ping_init();
    uart_IniT(115200);
    servo_init1();
    adc_init();

#if _PART1

    int cycle = 1;   // Represents how many times around the block we need to go
    int cycle_count = 0;
    int left_right = 0; //0 represents it goes to left and 1 represents right

    //allocating sensor data for movement of robot
    oi_t *Sensor_data = oi_alloc();
    //initializing the sensor data
    oi_init(Sensor_data);
    //creating and enumerator to cycle through our code
    enum
    {
        HOME, SETTINGS, CYCLE, DIRECTION, RUN, CONCLUDE, DONE
    } state;
    //while loop runs until the state equals DONE which is set at the very end of the code (line 379)
    /*****************************************
     *
     * ENTERING INTO THE MAIN LOOP OF THE PROGRAM
     *
     *******************************************/
    state = HOME;
    while (state != DONE)
    {

        //user menue homescreen is printed out
        lcd_printf("HOME\nb1: Setting\nb4: Run");
        //pressing button 1 takes the users into the settings menue screen
        if (button_num_ptr == 1)
        {
            state = SETTINGS;
            button_num_ptr = 0;
            while (state == SETTINGS)
            {
                //user is asked to input desired # of cycles and which direction the robot will travel (clockwise or counter-clockwise)
                lcd_printf(
                        "SETTING\nb1: # of cycles\nb2: Left or Right\nb4: Home");
                //Asking for # of cycles
                if (button_num_ptr == 1)
                {
                    state = CYCLE;
                    button_num_ptr = 0;
                    while (state == CYCLE)
                    {
                        lcd_printf(
                                "CYCLE\nb1: 1     b2: 2\nb3: 5     b4: Home");
                        //1 cycle
                        if (button_num_ptr == 1)
                        {
                            cycle = 1;
                            button_num_ptr = 0;
                            state = SETTINGS;
                        }
                        //2 cycles
                        else if (button_num_ptr == 2)
                        {
                            cycle = 2;

                            button_num_ptr = 0;
                            state = SETTINGS;
                        }
                        //5 cycles
                        else if (button_num_ptr == 3)
                        {
                            cycle = 3;

                            button_num_ptr = 0;
                            state = SETTINGS;
                        }
                        else if (button_num_ptr == 4)
                        {
                            //Do nothing -> Home
                            button_num_ptr = 0;
                            state = HOME;
                        }

                    }
                }

                //Asking user to choose travel direction

                if (button_num_ptr == 2)
                {
                    state = DIRECTION;
                    button_num_ptr = 0;
                    while (state == DIRECTION)
                    {
                        lcd_printf(
                                "LEFT or RIGHT\nb1: Left\nb2: Right\nb4: Home");
                        //Left
                        if (button_num_ptr == 1)
                        {
                            left_right = 0;
                            button_num_ptr = 0;
                            state = SETTINGS;
                        }
                        //Right
                        else if (button_num_ptr == 2)
                        {
                            left_right = 1;
                            button_num_ptr = 0;
                            state = SETTINGS;
                        }
                        else if (button_num_ptr == 4)
                        {
                            //Do nothing -> Home
                            button_num_ptr = 0;
                            state = HOME;
                        }
                    }
                }
                //if button 4 is pressed here the user is returned to the home screen
                if (button_num_ptr == 4)
                {
                    button_num_ptr = 0;
                    state = HOME;
                }
            }
        }

        //From the home screen, the user can press button 4 to run the program
        if (button_num_ptr == 4)
        {
            state = RUN;
            //creating a string to display which direction the robot will travel
            char LorR[6] = "Left";
            if (left_right)
            {
                LorR[0] = 'R';
                LorR[1] = 'i';
                LorR[2] = 'g';
                LorR[3] = 'h';
                LorR[4] = 't';
            }

            //printing the program status before the program starts
            lcd_printf("STATUS\n# of cycle: %d\nLeft or Right: %s\n", cycle,
                       LorR);
            timer_waitMillis(3000);
            lcd_printf("Walk Initialized");
            timer_waitMillis(500);
            lcd_printf("Walk in progress");
            /************************************************************************************
             *
             * Now the has started, the next chunk of code handles the robot during normal operations
             *
             *************************************************************************************/

            double distance = 0;            //tracks total distance traveled

            while (state == RUN) //while distance is less than 2000 (200cm), simply for testing purposes
            {
                //initializing the wheels to start driving
                oi_setWheels(400, 400);
                //updating the sensor data
                oi_update(Sensor_data);

                /******************************************************************************************************************************
                 *
                 *
                 * THIS SECTION OF CODE IS LOGICALLY IMPLEMENTED TO SCAN FOR OBJECTS WHILE WE ARE DRIVING,
                 * BUT IT DOES NOT WORK WE THINK BECAUSE OF HARDWAEAR ISSUES
                 *
                 *
                 *******************************************************************************************************************************/

                if (scan_detect() == 1 )
                {
                    turn(Sensor_data, -45);
                    move(Sensor_data, 20);
                    turn(Sensor_data, 45);
                    move(Sensor_data, 20);
                    turn(Sensor_data, -45);
                }
                else if (scan_detect() == 0)
                {
                    turn(Sensor_data, 45);
                    move(Sensor_data, 20);
                    turn(Sensor_data, -45);
                    move(Sensor_data, 20);
                    turn(Sensor_data, 45);
                }
                /**************************************************************************************************
                 *
                 *
                 * THIS IS WHERE THE CONDITIONAL CHECKS START FOR BUMPING INTO OBJECTS OR FALLING INTO HOLES/FINDING
                 * THE EDGE OF THE SIDEWALK
                 *
                 *
                 ***************************************************************************************************/
                //left bumper reaction
                if (Sensor_data->bumpLeft)
                {
                    lcd_printf("Bump");
                    cliffEscLeft(Sensor_data, distance);
                }
                //right bumbper reaction
                if (Sensor_data->bumpRight)
                {
                    lcd_printf("Bonk");
                    cliffEscRight(Sensor_data, distance);
                }
                //right cliff reaction
                if (Sensor_data->cliffRight)

                {
                    cliffEscRight(Sensor_data, distance);
                }
                //left cliff reaction
                if (Sensor_data->cliffLeft)

                {
                    cliffEscLeft(Sensor_data, distance);
                }

                //SPECIAL IF CONDITION TO CONTROLL DIRECTION OF WALK
                if (Sensor_data->cliffFrontLeft && Sensor_data->cliffFrontRight)
                {

                    move(Sensor_data, -20);
                    //implementing left_right variable for orientation of overall walk (walking in a clockwise or counter clockwise direction)

                    if (left_right)
                    {
                        //conditional for first encounter with edge of sidewalk
                        if (cycle_count == 0)
                        {
                            turn(Sensor_data, -89);
                        }
                        else
                        {
                            turn(Sensor_data, 89);
                        }
                    }
                    else
                    {
                        //conditional for first encounter with edge of sidewalk
                        if (cycle_count == 0)
                        {
                            turn(Sensor_data, 89);
                        }
                        else
                        {
                            turn(Sensor_data, -89);
                        }
                    }

                    //every time we hit an edge, cycle count increases by one to track progression status
                    cycle_count++;
                    lcd_printf("%d turns left",
                               5 + (4 * (cycle - 1)) - cycle_count);
                }

                /******************************************************************************
                 //TODO
                 //maybe implement a virtual wall feature if that works
                 //                if (sensor_data->virtualWall || sensor_data -> wallSensor)
                 //                {
                 //                    oi_setWheels(0, 0);

                 //}
                 *******************************************************************************/

                //check list to see how far we are in the cycle, preparing to look for home when cycle is complete
                if (cycle == 1 && cycle_count == 5)
                {
                    state = CONCLUDE;
                }
                if (cycle == 2 && cycle_count == 9)
                {
                    state = CONCLUDE;
                }
                if (cycle == 3 && cycle_count == 13)
                {
                    state = CONCLUDE;
                }

                /* if a white pole is detected (meaning another human is in the sidewalk) wait until the human moves
                 * in order to do the above we will have to use sonar and IR sensors to return width
                 *
                 * If the four pole home base is detected with IR and sonar, and the requisite amount of cycles have
                 * been completed, dog goes back inside house and robot stops
                 */
                else
                { //continue forward if no obstacle detected
                    distance += Sensor_data->distance;
                }
            }
            oi_setWheels(0, 0);

            while (state == CONCLUDE)
            {
                //this function serves to scan for the home base markers after the walk is completed
                //it specifically only expects to see two poles
                int objectCount = end_scan();
                timer_waitMillis(500);
                //this should turn the BOT to face in between the two poles and once it is aimed we can drive into home
                //turn(Sensor_data, angle);

                //This find two pair of poles
                if (objectCount >= 4)
                {
                    move(Sensor_data, 30);
                    //left bumper reaction
                    if (Sensor_data->bumpLeft)
                    {
                        lcd_printf("Bump");
                        cliffEscLeft(Sensor_data, distance);
                    }
                    //right bumper reaction
                    if (Sensor_data->bumpRight)
                    {
                        lcd_printf("Bonk");
                        cliffEscRight(Sensor_data, distance);
                    }
                }
                if (objectCount <= 3)
                {
                    if (left_right)
                    {
                        turn(Sensor_data, -89);
                        distance = 0;
                        while (distance < 400) //while distance is less than 2000 (200cm), simply for testing purposes
                        { //Eventually, once testing is done, this will have to be based on something like number of turns made
                            oi_setWheels(400, 400);
                            oi_update(Sensor_data);

                            if (Sensor_data->bumpRight)
                            { //escape right due to left side cliff
                                move(Sensor_data, -20);
                                turn(Sensor_data, -89);
                                move(Sensor_data, 20);
                                distance = distance - 200; //accounts for the distance counted during lateral movement
                                turn(Sensor_data, 89);
                                move(Sensor_data, 20);
                                turn(Sensor_data, -89);
                                break;
                            }
                            if (Sensor_data->bumpLeft)
                            { //escape left due to right side cliff
                                move(Sensor_data, -20);
                                turn(Sensor_data, 89);
                                move(Sensor_data, 20);
                                distance = distance - 200; //accounts for the distance counted during lateral movement
                                turn(Sensor_data, -89);
                                move(Sensor_data, 20);
                                turn(Sensor_data, 89);
                                break;
                            }
                            else
                            { //continue forward if no obstacle detected
                                distance += Sensor_data->distance;
                            }
                        }
                        break;
                    }
                    if (!left_right)
                    {
                        turn(Sensor_data, 89);
                        distance = 0;
                        while (distance < 400) //while distance is less than 2000 (200cm), simply for testing purposes
                        { //Eventually, once testing is done, this will have to be based on something like number of turns made
                            oi_setWheels(400, 400);
                            oi_update(Sensor_data);

                            if (Sensor_data->bumpRight)
                            { //escape right due to left side cliff
                                move(Sensor_data, -20);
                                turn(Sensor_data, -89);
                                move(Sensor_data, 20);
                                distance = distance - 200; //accounts for the distance counted during lateral movement
                                turn(Sensor_data, 89);
                                move(Sensor_data, 20);
                                turn(Sensor_data, -89);
                                break;
                            }
                            if (Sensor_data->bumpLeft)
                            { //escape left due to right side cliff
                                move(Sensor_data, -20);
                                turn(Sensor_data, 89);
                                move(Sensor_data, 20);
                                distance = distance - 200; //accounts for the distance counted during lateral movement
                                turn(Sensor_data, -89);
                                move(Sensor_data, 20);
                                turn(Sensor_data, 89);
                                break;
                            }
                            else
                            { //continue forward if no obstacle detected
                                distance += Sensor_data->distance;
                            }
                        }
                        break;
                    }
                }
                else
                {
                    move(Sensor_data, 20);
                }
            }
            oi_setWheels(0, 0);
            oi_free(Sensor_data);
            state = DONE;
        }
    }

#endif

#endif
}

