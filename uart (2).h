/*
*
*   uart.h
*
*   Used to set up the UART
*   uses UART1 at 115200
*
*
*   @author Dane Larson
*   @date 07/18/2016
*   Phillip Jones updated 9/2019, removed WiFi.h
*/

#ifndef UART_LAB6_H_
#define UART_LAB6_H_

#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

extern volatile  char flag;  // Your UART interupt code can place read data here
extern volatile char uart_data;

void uart_IniT(int baud);

void send_char(char data);

int uart_ReceivE(void);

void uart_SendStR(const char *data);




#endif /* UART_LAB6_H_ */
