/* 
 * File:   fuses.h
 * Author: peterl
 *
 * Created on February 19, 2018, 11:07 AM
 * 
 * This file contains non-volatile configuration bit settings for the MCU.
 */

#ifndef FUSES_H
#define	FUSES_H

#include "sfrdefs.h"

/* Configuration bits (Strom)   */
#pragma config OSC      = INTIO67
#pragma config STVREN   = OFF
#pragma config PWRT     = ON
#pragma config BOREN    = OFF
#pragma config WDT      = OFF
#pragma config PBADEN   = OFF
#pragma config CCP2MX   = PORTC
#pragma config MCLRE    = OFF
#pragma config LVP      = OFF

/* Additional config bits       */
#pragma config XINST    = OFF

#endif	/* FUSES_H */

