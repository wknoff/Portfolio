/* 
 * File:   motor_driver.h
 * Author: wknoff
 * Description: contains functions necessary to drive motor, use LED, and communicate over UART using a PIC32 board
 * Created on November 3, 2023, 12:23 PM
 */

#include "motor_driver.h"
#include <xc.h>

void lcd_display_driver_enable()
{
    //Enable bit is controlled by pin 81
   
    LATDbits.LATD4 = 1;
   
    //Using for loop to delay, then setting enable bit back to 0
    int i = 5000;
    for(i; i>0; i--);
    LATDbits.LATD4 = 0;
}


void lcd_display_driver_initialize()
{
    //Set RS and RW to 0
    LATBbits.LATB15 = 0;
    LATDbits.LATD5 = 0;
   
    int i = 5000;
    for(i; i>0; i--); //Delaying for more than 30 ms after VDD on
    LATE = 0b00111000; //Setting LCD to 2-line, 5x7 mode
    lcd_display_driver_enable();
    for(i; i>0; i--); //Delaying for more than 40 microseconds  
    LATE = 0b00001100; //Setting LCD to display on, cursor off, blink off
    lcd_display_driver_enable();
    for(i; i>0; i--); //Delaying for more than 40 microseconds  
    lcd_display_driver_clear();
    for(i; i>0; i--); //Delaying for more than 1.64ms
    LATE = 0x00000110; //Setting LCD to increment mode, shift off
    lcd_display_driver_enable();
}

//clears the LCD Screen
void lcd_display_driver_clear()
{
    LATE = 0x01;
    lcd_display_driver_enable();
}

//WRites text to the LCD screen
void lcd_display_driver_write(char* data, int length)
{
    LATBbits.LATB15 = 0; //setting RW
    int i;
    for(i=0; i < length; i++) //iterates through and writes each letter one at a time
    {
        LATBbits.LATB15 = 1; //setting RW
        LATE = data[i]; //prints that letter
        lcd_display_driver_enable();
    }
    LATBbits.LATB15 = 0; //setting RW
}


void display_driver_use_first_line(void)
{
    //Set RS and RW to 0
    LATBbits.LATB15 = 0;
    LATDbits.LATD5 = 0;
    LATE = 0x80;
    lcd_display_driver_enable();
}


void display_driver_use_second_line(void)
{
    //Set RS and RW to 0
    LATBbits.LATB15 = 0;
    LATDbits.LATD5 = 0;
    LATE = 0xC0;
    lcd_display_driver_enable();
}

unsigned int read_potentiometer(void)
{
    AD1CON1bits.ADON = 1;
    AD1CHSbits.CH0SA = 2; //read from analog input 2, the temp sensor
    AD1CON3bits.ADCS = 2; //board specific for analog to digital
    AD1CON1bits.SAMP = 1; //start sampling
    unsigned int elapsed = _CP0_GET_COUNT(); //store core timer value
    unsigned int end_time = elapsed + 10000; //when the capacitor should be done charging
    while(_CP0_GET_COUNT() < end_time); //iterates until the core timer reaches end time
    AD1CON1bits.SAMP = 0; //end sampling
    while(!AD1CON1bits.DONE); //iterating until it finished
   
    return ADC1BUF0;
}


float uart_receive(char* msg)
{
    char data = 0;
    int complete = 0;
    int byte = 0;
    
    while(!complete)
    {
        if(U2STAbits.URXDA){
            data = U2RXREG;
            if((data == '\r') || (data == '\n'))
            {
                complete = 1;
            }
            else
            {
                msg[byte] = data;
                byte++;
            }
        }
        msg[byte] = '\0'; //null character
    }
}

void uart_write(char* timeStr, char* refAngleStr, char* currentAngleStr)
{
    while(*timeStr != '\0')
    {
        while(U2STAbits.UTXBF); //waiting until buffer isn't full
        U2TXREG = *timeStr;
        ++timeStr;
    }
    
    while(*refAngleStr != '\0')
    {
        while(U2STAbits.UTXBF); //waiting until buffer isn't full
        U2TXREG = *refAngleStr;
        ++refAngleStr;
    }
    
    while(*currentAngleStr != '\0')
    {
        while(U2STAbits.UTXBF); //waiting until buffer isn't full
        U2TXREG = *currentAngleStr;
        ++currentAngleStr;
    }
}

void motor_direction(int direction)
{
    if(direction == 0) //forward direction
    {
        /* For some reason, the direction was reversed when using the lab given values
        LATFbits.LATF0 = 1;
        LATFbits.LATF1 = 0;
        */
        LATFbits.LATF0 = 0;
        LATFbits.LATF1 = 1;
    }
    else if(direction == 1) //reverse direction
    {
        /* Same issue as before
        LATFbits.LATF0 = 0;
        LATFbits.LATF1 = 1;
        */
        LATFbits.LATF0 = 1;
        LATFbits.LATF1 = 0;
    }
    else if(direction == 2) //stop motor
    {
        LATFbits.LATF0 = 0;
        LATFbits.LATF1 = 0;
    }
}