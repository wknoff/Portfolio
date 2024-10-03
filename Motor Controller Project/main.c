/* 
 * File:   main.c
 * Author: wknoff
 * Description: C program to to perform On/Off DC Motor Control
 * Created on November 3, 2023, 12:00 PM
 */

//Sets system and peripheral bus clock to run at 80MHz
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF 
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

#include <xc.h>
#include <sys/attribs.h> //used for interrupts
#include "motor_driver.h"
#include <stdio.h>
#include <stdlib.h> //used for sprintf() function
#include <string.h> //used for strlen())

#define PGain 4.15 //proportional gain value
#define PIGain 0.03 //proportional integral gain

//Global variables
static volatile int ticks = 0; //angle
static volatile float time = 0.0; //used to keep track of time after reference angle is received
static volatile float desiredAngle; //angle received via UART
static volatile float currentAngle; //keeps track of current angle
static volatile int TXcount = 0; //used to stop the board from transmitting more than 500 samples
static volatile int controller_mode = 0; //0 means on/off DC control, 1 means proportional DC control
float integralError = 0;

//used for saving float data to strings
char timeStr[10];
char desiredAngleStr[10];
char currentAngleStr[10];

//For state machine
int statebit1 = 0;
int statebit2 = 0;

//FOR TESTING POTENTIOMETER: unsigned int potread=0;

void __ISR( _CHANGE_NOTICE_VECTOR, IPL5SOFT) CNISR ( void )
{
    if(PORTDbits.RD6 == 0) //checks if S3 is pressed
    {
        controller_mode = 1; //proportional DC control
        LATA = 0x0000;
        LATAbits.LATA1 = 1;
    }
    else if(PORTDbits.RD13 == 0) //checks if S4 is pressed
    {
        controller_mode = 0; //on/off DC control
        LATA = 0x0000;
        LATAbits.LATA0 = 1;
    }
    
    //State machine code
    if(statebit1 == 0 && statebit2 == 0)
    {
        if(PORTGbits.RG6 == 1)
        {
            statebit1 = 1; //changing state
            ticks--; //decrements to keep track of rotation
        }
        else if(PORTGbits.RG7 == 1)
        {
            statebit2 = 1; //changing state
            ticks++; //increments to keep track of rotation
        }
    }
    else if(statebit1 == 0 && statebit2 == 1)
    {
        if(PORTGbits.RG6 == 1)
        {
            statebit1 = 1; //changing state
            ticks++; //increments to keep track of rotation
        }
        else if(PORTGbits.RG7 == 0)
        {
            statebit2 = 0; //changing state
            ticks--; //decrements to keep track of rotation
        }
    }
    else if(statebit1 == 1 && statebit2 == 0)
    {
        if(PORTGbits.RG6 == 0)
        {
            statebit1 = 0; //changing state
            ticks++; //increments to keep track of rotation
        }
        else if(PORTGbits.RG7 == 1)
        {
            statebit2 = 1; //changing state
            ticks--; //decrements to keep track of rotation
        }
    }
    else if(statebit1 == 1 && statebit2 == 1)
    {
        if(PORTGbits.RG6 == 0)
        {
            statebit1 = 0; //changing state
            ticks--; //decrements to keep track of rotation
        }
        else if(PORTGbits.RG7 == 0)
        {
            statebit2 = 0; //changing state
            ticks++; //increments to keep track of rotation
        }
    }
    IFS1bits.CNIF = 0; //setting flag to 0
}


void __ISR( _UART2_VECTOR, IPL3SOFT) UART2Handler ( void )
{
    char msg[10];
    uart_receive(msg); //calling receive function
    desiredAngle = atof(msg); //convert it from string to float

    T5CONbits.ON = 1; //Turning on timer 5 interrupt for writing
    T2CONbits.ON = 1; //TUrning on timer 2
    T3CONbits.ON = 1; //Turning on timer 3
    OC4CONbits.ON = 1; //Turning on output compare
    
    _CP0_SET_COUNT(0); //start counting
    IFS1bits.U2RXIF = 0; //clearing flag
}

void __ISR( _TIMER_5_VECTOR, IPL2SOFT ) Timer5ISR( void ){
    time = _CP0_GET_COUNT() * 0.000000025; //core timer runs at 40MHz
    currentAngle = ticks*0.07575; //calculating current angle
    
    //saving float data to string
    sprintf(timeStr,"%0.4f\r\n", time);
    sprintf(desiredAngleStr,"%0.4f\r\n", desiredAngle);
    sprintf(currentAngleStr,"%0.4f\r\n", currentAngle);
    
    uart_write(timeStr, desiredAngleStr, currentAngleStr); //writing all three values
    
    //Prevents the program from transmitting after 125 samples have been transmitted
    TXcount++;
    if(TXcount >= 125)
    {
        T5CONbits.ON = 0; //disabling timer 5 interrupt
    }
    
    IFS0bits.T5IF = 0; //clearing flag
}

//Maps to S5
void __ISR( _EXTERNAL_2_VECTOR, IPL1SOFT ) EXT2ISR (void ){
    if(PORTAbits.RA7 == 0) //checks if S5 is pressed
    {
        controller_mode = 2; //proportional integral DC control
        LATA = 0x0000;
        LATAbits.LATA2 = 1; 
    }
    
    IFS0bits.INT2IF = 0; // Clear the INT2 interrupt flag
}

void __ISR( _TIMER_2_VECTOR, IPL4SOFT ) Timer2ISR( void ){
    if(controller_mode == 0) // on/off
    {
        OC4RS = read_potentiometer(); //potentiometer determines duty cycle
        //FOR TESTING POTENTIOMETER: potread = read_potentiometer();

        if(abs(desiredAngle - currentAngle) <= 7.5) //stop motor
        {
            motor_direction(2);
        } 
        else if(desiredAngle > currentAngle) //rotate forwards, increment
        {
            motor_direction(0);
        } 
        else //rotate backwards, decrement
        {
            motor_direction(1);
        }
    }
    else if(controller_mode == 1) //proportional
    {
        //calculate the error
        float error = desiredAngle - currentAngle;

        //calculate the control output
        float controlOutput = PGain * error;

        //control the direction of the motor
        if (controlOutput < 0) {
            motor_direction(1); // Spin backwards
        } else if (controlOutput > 0) {
            motor_direction(0); // Spin forwards
        } else {
            motor_direction(2); // Stop
        }

        //scale the PWM signal to a max of 1023
        if (abs(controlOutput) > 1023) {
            OC4RS = 1023;
        }
        else
        {
            OC4RS = abs(controlOutput);
        }
    }
    else
    {
        float samplingPeriod = 0.02; // 1/50
        
        float error = desiredAngle - currentAngle;
        
        integralError += error * samplingPeriod;
        
        float controlOutput = PGain * error + PIGain * integralError;
        
        //control the direction of the motor
        if (controlOutput < 0) {
            motor_direction(1); // Spin backwards
        } else if (controlOutput > 0) {
            motor_direction(0); // Spin forwards
        } else {
            motor_direction(2); // Stop
        }

        //scale the PWM signal to a max of 1023
        if (abs(controlOutput) > 1023) {
            OC4RS = 1023;
        }
        else
        {
            OC4RS = abs(controlOutput);
        }
    }
   
    IFS0bits.T2IF = 0; //clearing flag
}

int main(void) 
{
    //Setting tristate register assignments
    TRISBbits.TRISB15 = 0;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISE = 0xFF00;
    TRISFbits.TRISF0 = 0;
    TRISFbits.TRISF1 = 0;
    
    //LEDs for testing functionality
    DDPCONbits.JTAGEN = 0;
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    LATA = 0x0000;
    LATAbits.LATA0 = 1;
    
    INTCONbits.MVEC = 1; //enable multi-vector mode
    __builtin_disable_interrupts();
    
    //Change notification ISR settings
    CNCONbits.ON = 1; //turns on change notification interrupts
    IPC6bits.CNIP = 5; //setting maximum priority
    IFS1bits.CNIF = 0; //setting flag to 0
    IEC1bits.CNIE = 1; //enabling CN 
    CNENbits.CNEN8 = 1; //encoder A output
    CNENbits.CNEN9 = 1; //encoder B output
    CNENbits.CNEN19 = 1; //push button S4
    CNENbits.CNEN15 = 1; //push button S3
    
    //UART ISR settings
    U2MODEbits.ON = 1; //turns on UART2
    IPC8bits.U2IP = 3; //setting priority. Manual says IPC7 but that could be an error
    IFS1bits.U2RXIF = 0; //setting flag to 0
    IEC1bits.U2RXIE = 1; //enabling RX UART2 interrupt
    
    //Time 5 ISR Settings
    PR5 = (80000000/(256*25))-1; //calculating period register given 256 prescaler and desired 25hz
    T5CONbits.ON = 0; //Turning on time 5
    T5CONbits.TCKPS = 0b111; //setting prescalar to 256
    IPC5bits.T5IP = 2; //priority
    IFS0bits.T5IF = 0; //setting flag to 0
    IEC0bits.T5IE = 1; //enabling timer 5 interrupt
    
    PR2 = (80000000/(256*50))-1; //calculating period register given 256 prescaler and desired 50hz
    T2CONbits.ON = 0; //Turning off timer 2
    T2CONbits.TCKPS = 0b111; //setting prescalar to 256
    IPC2bits.T2IP = 4; //priority
    IFS0bits.T2IF = 0; //setting flag to 0
    IEC0bits.T2IE = 1; //enabling timer 2 interrupt

    INTCONbits.INT2EP = 0; //triggers on falling edge
    IPC2bits.INT2IP = 1; //setting maximum priority
    IFS0bits.INT2IF = 0; //setting flag to 0
    IEC0bits.INT2IE = 1; //enable interrupt
 
    __builtin_enable_interrupts();
    
    //setting ADC input outputs
    AD1PCFGbits.PCFG2 = 0;
    AD1PCFGbits.PCFG4 = 0;
    
    //Setting up display
    lcd_display_driver_initialize();
    lcd_display_driver_clear();
    
    //setting up UART
    U2MODEbits.UEN = 0; //hardware flow control off
    U2MODEbits.PDSEL = 0b00; //sets it to 8 bits no parity
    U2MODEbits.STSEL = 0; //one stop bit
    U2STAbits.URXEN = 1; //enable UART2 RX
    U2STAbits.UTXEN = 1; //enable UART2 TX
    U2MODEbits.BRGH = 0; //setting M=16
    U2BRG = (80000000/(16*230400))-1; //Setting baud rate
    U2STAbits.URXISEL = 0b00; //only one bit needed to trigger RX interrupt

    //setting up Timer3
    PR3 = 1022;
    T3CONbits.TCKPS = 0b011; //setting prescalar to 8
    T3CONbits.ON = 0; //Turning on timer 3
    
    //setting up output compare
    OC4CONbits.OCTSEL = 1; //using 16-bit timer 3
    OC4CONbits.OCM = 0b110; //PWM Mode fault pin disabled
    OC4RS = 0; //when I set it to 1023, as the lab says, my motor moves for a split second even when the potentiometer is set to 0. I've set it to 0 here to prevent this issue
    OC4R = 1023;
    OC4CONbits.ON = 0; //disabling output compare
    
    while(1)
    {
        //printing reference angle to first line
        char angleprint[20]; 
        sprintf(angleprint, "Ref: %10.2f%c", desiredAngle, 0xDF); 
        //FOR TESTING POTENTIOMETER: sprintf(angleprint, "Ref:%2.0f%c Pot:%4u", desiredAngle, 0xDF, potread); for testing potentiometer
        display_driver_use_first_line();
        lcd_display_driver_write(angleprint, strlen(angleprint));
        
        //printing current angle to second line
        currentAngle = ticks*0.07575; //calculating current angle
        char currentangleprint[20]; 
        sprintf(currentangleprint, "Angle: %8.2f%c", currentAngle, 0xDF);
        display_driver_use_second_line();
        lcd_display_driver_write(currentangleprint, strlen(currentangleprint)); //printing current angle
    }
}