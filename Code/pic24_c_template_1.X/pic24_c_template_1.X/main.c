/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__PIC24E__)
#include <p24Exxxx.h>
#elif defined(__PIC24F__) || defined(__PIC24FK__)
#include <p24Fxxxx.h>
#elif defined(__PIC24H__)
#include <p24Hxxxx.h>
#endif
#endif

#include <stdint.h>  /* Includes uint16_t definition                    */
#include <stdbool.h> /* Includes true/false definition                  */
#include <stdio.h>   /* Includes printf                                 */

#include "system.h" /* System funct/params, like osc/peripheral config */
#include "user.h"   /* User funct/params, such as InitApp              */

//#pragma config WDTE = 0
//#pragma config LVP = 0
#define CYCLES_PER_MS ((uint16_t)(FCY * 0.001))

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Prototype declarations                                                              */
/******************************************************************************/
void delay(uint16_t d); // prototype declaration for delay function
void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void);
void configTimer1(void);

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
int16_t main(void)
{

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    /* TODO <INSERT USER APPLICATION CODE HERE> */
    configTimer1();

    TRISBbits.TRISB12 = 0; // This sets pin RB12 to output

    while (1) // Infinite loop
    {
        //PORTBbits.RB12 = 1; // Set RB12 pin to logic high (5V)
        //delay_ms(3000); // Delay for roughly 500ms
        PORTBbits.RB12 = 0; // Set RB12 pin to logic low (0V)
        //delay_ms(3000); // Delay for roughly 500ms
    }
    return 0;
}

void delay_ms(uint16_t d)
{
    uint16_t ms = CYCLES_PER_MS * d;
    __delay32(ms);
}

void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0; //Reset Timer1 interrupt flag and Return from ISR
}

void configTimer1(void)
{
    T1CON = 0x00;         //Stops the Timer1 and reset control reg.
    TMR1 = 0x00;          //Clear contents of the timer register
    PR1 = 0xFFFF;         //Load the Period register with the value 0xFFFF
    IPC0bits.T1IP = 0x01; //Setup Timer1 interrupt for desired priority level
                          // (This assigns level 1 priority)
    IFS0bits.T1IF = 0;    //Clear the Timer1 interrupt status flag
    IEC0bits.T1IE = 1;    //Enable Timer1 interrupts
    T1CONbits.TON = 1;    //Start Timer1 with prescaler settings at 1:1 and
                          //clock source set to the internal instruction cycle
}