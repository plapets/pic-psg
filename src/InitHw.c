
#include "InitHw.h"

/* InitHwMinimal
 *  This function configures the PIC to run at 32 MHz and sets initial port
 *  states.  This should be the first function called after the entry point.
 */
void InitHwMinimal(void)
{
    /* Configure oscillator.                                                */
    OSCCON             |= 0x70;     /* Set the PIC to run at 8 MHz.         */
    OSCTUNEbits.PLLEN   = 1;        /* Enable the PLL to run at 32 MHz.     */
    
    /* Configure interrupt registers.                                       */
    INTCON  =   _INTCON_PEIE_GIEL_MASK  ;   /* Enable low priority int.'s.  */
    
    INTCON2 =   _INTCON2_NOT_RBPU_MASK  |   /* Disable pull-ups on port B   */
                _INTCON2_INTEDG0_MASK   |   /* Set external interrupt on    */
                _INTCON2_INTEDG1_MASK   |   /* rising edge. (unused)        */
                _INTCON2_INTEDG2_MASK   ;   /*  ...                         */
    
    INTCON3 =   0;                          /* Not using external int.'s    */
    RCON   |=   _RCON_IPEN_MASK;            /* Use interrupt priorities.    */
    
    /* Set default states for ports.                                        */
    ADCON1  = 0x0F;                 /* Disable A-to-D ports.                */
    
    LATA    = 0x00;                 /* Set PORT A data latch LOW.           */
    TRISA   = 0xFF;                 /* Set PORT A pins as inputs.           */
    
    LATB    = 0x00;                 /* Set PORT B data latch LOW.           */
    TRISB   = 0xFF;                 /* Set PORT B pins as inputs.           */
    
    LATC    = 0x00;                 /* Set PORT C data latch LOW.           */
    TRISC   = 0xFF;                 /* Set PORT C pins as inputs.           */
}
