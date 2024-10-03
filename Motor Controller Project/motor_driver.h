/* 
 * File:   motor_driver.h
 * Author: wknoff
 * Description: contains function prototypes necessary to drive motor, use LED, and communicate over UART using a PIC32 board
 * Created on November 3, 2023, 11:57 AM
 */

#ifndef MOTOR_DRIVER_H

//Toggles the enable bit long enough to perform a display operation
void lcd_display_driver_enable();

//Performs initialization sequence for LCD, setting it to 5x7 dots, dual line, no cursor
void lcd_display_driver_initialize();

//clears characters from display
void lcd_display_driver_clear();

//Writes string data of a certain length to display
void lcd_display_driver_write(char* data, int length);

//Sets display to write to first line
void display_driver_use_first_line(void);

//Sets display to write to second line
void display_driver_use_second_line(void);

unsigned int read_potentiometer();

float uart_receive(char* msg);

void uart_write(char* timeStr, char* refAngleStr, char* currentAngleStr);

void motor_direction(int direction);

#endif	/* MOTOR_DRIVER_H */