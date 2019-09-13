/* 
 * File:   isr.h
 * Author: peterl
 *
 * Created on April 17, 2018, 8:23 PM
 * 
 * This file declares the high- and low-priority interrupt routines used by the
 * programmable sound generator.  The high-priority ISR is called to service
 * the SPI slave module.  The low-priority ISR is used for wave and envelope
 * generation.
 */

#ifndef ISR_H
#define	ISR_H

#include "sfrdefs.h"

#include "Spi.h"
#include "SoundGenHw.h"

/* SPI handling (SSP, RB pin change) */
#ifdef __SDCC
void isr_high_priority(void) __interrupt 1;
#elif __XC8
void __high_priority __interrupt isr_high_priority(void);
#endif

/* Envelope and wave timers (Timer0, CCP1, CCP2) */
#ifdef __SDCC
void isr_low_priority(void) __interrupt 2;
#elif __XC8
void __low_priority __interrupt isr_low_priority(void);
#endif

#endif	/* ISR_H */

