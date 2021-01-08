/*
*   uart.c
*
*   @author
*   @date
*/

#include "uart.h"
#include "timer.h"
#include "lcd.h"

void uart_IniT(int baud){

	SYSCTL_RCGCGPIO_R |= 0b000010;
    SYSCTL_RCGCUART_R |= 0b00000010;
	GPIO_PORTB_AFSEL_R |= 0b00000011;
	GPIO_PORTB_PCTL_R |= 0x11;     // Force 1's in the disired locations
	GPIO_PORTB_DEN_R |= 0b00000011;
	//GPIO_PORTB_DIR_R &= 0b11111110;      // Force 0's in the disired locations
	//GPIO_PORTB_DIR_R |= 0b00000010;
	GPIO_PORTB_DIR_R |= 0x01;


    //computing baud values
    double fbrd;
    int ibrd;


    fbrd = 44;
    ibrd = 8;

    UART1_CTL_R &= 0xFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R = ibrd;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = fbrd;   // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0b01100000;        // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R   = 0b00000000;          // use system clock as clock source (page 939)
    UART1_CTL_R |= 0b00000001;
}

void send_char(char data){
	while((UART1_FR_R & 0x20) != 0){

	}
	UART1_DR_R = data;


}

int uart_ReceivE(void){
	int ret;

	while(UART1_FR_R & 0x10){

    }
        ret = UART1_DR_R;
    return ret;
}

void uart_SendStR(const char data[]){
	//int i = 0;
    while(*data != '\0'){

	    send_char(*data);
	    data++;
	}
}




