/* 
 * File:   sdcc.h
 * Author: peterl
 *
 * Created on February 19, 2018, 5:33 AM
 * 
 * This file contains miscellaneous definitions required for compilation using
 * various tools.
 */

#ifndef SFRDEFS_H
#define	SFRDEFS_H

#ifdef __SDCC
#include <pic18fregs.h>

/* Copied verbatim from pic18f2520.h */

#define _INTCON_GIE_GIEH_MASK                               0x80
#define _INTCON_PEIE_GIEL_MASK                              0x40
#define _INTCON_TMR0IE_MASK                                 0x20
#define _INTCON_RBIE_MASK                                   0x8
#define _INTCON_TMR0IF_MASK                                 0x4
#define _INTCON_RBIF_MASK                                   0x1


#define _INTCON2_NOT_RBPU_MASK                              0x80
#define _INTCON2_INTEDG0_MASK                               0x40
#define _INTCON2_INTEDG1_MASK                               0x20
#define _INTCON2_INTEDG2_MASK                               0x10
#define _INTCON2_RBIP_MASK                                  0x1

#define _PIE1_SSPIE_MASK                                    0x8
#define _PIE1_CCP1IE_MASK                                   0x4
#define _PIE1_TMR2IE_MASK                                   0x2
#define _PIE1_TMR1IE_MASK                                   0x1

#define _PIE2_TMR3IE_MASK                                   0x2
#define _PIE2_CCP2IE_MASK                                   0x1


#define _PIR1_SSPIF_MASK                                    0x8
#define _PIR1_CCP1IF_MASK                                   0x4
#define _PIR1_TMR2IF_MASK                                   0x2
#define _PIR1_TMR1IF_MASK                                   0x1

#define _PIR2_TMR3IF_MASK                                   0x2
#define _PIR2_CCP2IF_MASK                                   0x1

#define _IPR1_SSPIP_MASK                                    0x8
#define _IPR1_CCP1IP_MASK                                   0x4
#define _IPR1_TMR1IP_MASK                                   0x1

#define _IPR2_TMR3IP_MASK                                   0x2
#define _IPR2_CCP2IP_MASK                                   0x1

#define _RCON_IPEN_MASK                                     0x80

#define _T0CON_TMR0ON_MASK                                  0x80
#define _T0CON_T08BIT_MASK                                  0x40
#define _T0CON_T0PS2_MASK                                   0x4
#define _T0CON_T0PS0_MASK                                   0x1

#define _T1CON_RD16_MASK                                    0x80
#define _T1CON_T1CKPS1_MASK                                 0x20
#define _T1CON_T1CKPS0_MASK                                 0x10
#define _T1CON_TMR1ON_MASK                                  0x1

#define _T2CON_TMR2ON_MASK                                  0x4
#define _T2CON_T2CKPS1_MASK                                 0x2

#define _T3CON_RD16_MASK                                    0x80
#define _T3CON_T3CKPS1_MASK                                 0x20
#define _T3CON_T3CKPS0_MASK                                 0x10
#define _T3CON_T3CCP1_MASK                                  0x8
#define _T3CON_TMR3ON_MASK                                  0x1

#define _CCP1CON_CCP1M3_MASK                                0x8
#define _CCP1CON_CCP1M2_MASK                                0x4
#define _CCP1CON_CCP1M1_MASK                                0x2
#define _CCP1CON_CCP1M0_MASK                                0x1

#define _CCP2CON_CCP2M3_MASK                                0x8
#define _CCP2CON_CCP2M2_MASK                                0x4
#define _CCP2CON_CCP2M1_MASK                                0x2
#define _CCP2CON_CCP2M0_MASK                                0x1

#define _SSPSTAT_CKE_MASK                                   0x40
#define _SSPSTAT_CKE_POSN                                   0x6

#define _SSPCON1_SSPEN_MASK                                 0x20
#define _SSPCON1_CKP_POSN                                   0x4

#define _RCSTA_SPEN_MASK                                    0x80
#define _TXSTA_TXEN_MASK                                    0x20

/* End verbatim copied defines */

#define	ei()	(INTCON |=  _INTCON_GIE_GIEH_MASK)
#define	di()	(INTCON &= ~_INTCON_GIE_GIEH_MASK)

#elif __XC8
#include <xc.h>
#endif

#define _BV(n)  (1 << (n))
#define NULL    0

#endif	/* SFRDEFS_H */

