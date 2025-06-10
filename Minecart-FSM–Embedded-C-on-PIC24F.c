/*
 * File:   Exercise_1_main.c
 * Author: Abdelrahman Abdalla
 *
 * 
 */

/* define FCY before including libpic30.h */
#define FCY 3685000UL

#include "xc.h"
#include "libpic30.h"

#pragma config ICS = PGD2 // Communicate on PGED2 (pin 14) and PGEC2 (pin 15) 

/* declare functions */
void initialize_IO_ports (void);
void initialize_INT(void);
void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt (void);

typedef enum {
    S0, S1, S2, S3 
} STATES;    // add all the required states

/* define states outside of main so that it can be accessed by the ISR */
STATES state = S0; /* set initial state to S0 */

void initialize_IO_ports (void) {
    /* Configure pins 2 and 3 for digital I/O */
    ANSELAbits.ANSA0 = 0;  // use pin 2 (RA0) for input E
    ANSELAbits.ANSA1 = 0;  // use pin 3 (RA1) for input L
    ANSELBbits.ANSB0 = 0; // use pin 4 (RPI32) for input R
    /* Set digital I/O data direction */
    TRISAbits.TRISA0 = 1;  // configure RA0 (pin 2) for input E
    TRISAbits.TRISA1 = 1;  // configure RA1 (pin 3) for input L
    TRISBbits.TRISB0 = 1;  // configure RPI32 (pin 4) for input R
    TRISBbits.TRISB15 = 0; // configure RB15 (pin 26) for output X
    TRISBbits.TRISB14 = 0; // configure RB14 (pin 25) for output Y
    TRISBbits.TRISB13 = 0; //configure RB13 (pin 24) for output Z)
  }


  void initialize_INT (void) {
      RPINR0bits.INT1R = 0x20; // assign INT1 to RPI32 (pin 4)
      INTCON2bits.GIE = 1;     // set global interrupt enable
      INTCON2bits.INT1EP = 1;      // interrupt on negative edge
      IFS1bits.INT1IF = 0;     // clear interrupt flag
      IEC1bits.INT1IE = 1;     // enable interrupt
  }    

  void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt (void) {
      
      LATBbits.LATB13 = 0;  //set output Z to 0
      LATBbits.LATB14 = 0;  //set output Y to 0
      LATBbits.LATB15 = 0;  //set output Z to 0
      state = S0;          // set state to S0
      IFS1bits.INT1IF = 0; // clear interrupt flag
                            
  }
  
int main(void) {
    int input;

    initialize_IO_ports ();
    initialize_INT ();

    while (1) {// infinite loop 
       
       input = (!PORTAbits.RA1 << 1) | !PORTAbits.RA0;  // read input

       switch (state) {// determine next state
            case S0:                                // case 0 --> L = 0, E = 0
                switch (input) {                    // case 1 --> L = 0, E = 1
                    case 0: state = S0; break;      // case 2 --> L = 1, E = 0
                    case 1: state = S1; break;      // case 3 --> L = 1, E = 1
                    case 2: state = S0; break;
                    case 3: state = S0; break;
                }; break;
            case S1:
                switch (input) {
                    case 0: state = S1; break;
                    case 1: state = S2; break;
                    case 2: state = S0; break;
                    case 3: state = S1; break;
                }; break;
            case S2:
                switch (input) {
                    case 0: state = S3; break;
                    case 1: state = S2; break;
                    case 2: state = S1; break;
                    case 3: state = S2; break;
                }; break;
            case S3:
                switch (input) {
                    case 0: state = S0; break;
                    case 1: state = S0; break;
                    case 2: state = S0; break;
                    case 3: state = S0; break;
                }; break;
        }
        
        switch (state) {// set output
            case S0:
                LATBbits.LATB15 = 0;         // 15 = x
                LATBbits.LATB14 = 0;         // 14 = y
                LATBbits.LATB13 = 0; break;  // 13 = z
            case S1:
                LATBbits.LATB15 = 1;   
                LATBbits.LATB14 = 0; 
                LATBbits.LATB13 = 0; break;
            case S2:
                LATBbits.LATB15 = 0;   
                LATBbits.LATB14 = 1;
                LATBbits.LATB13 = 0; break;
            case S3:
                LATBbits.LATB15 = 0; 
                LATBbits.LATB14 = 1; 
                LATBbits.LATB13 = 1; break;
        }
         __delay_ms(1000); // delay 1 sec
        ClrWdt();          // restart the watchdog timer
    }
    return 0;
}




