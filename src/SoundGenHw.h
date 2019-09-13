/* 
 * File:   SoundGenHw.h
 * Author: peterl
 *
 * Created on April 17, 2018, 9:09 AM
 * 
 * This file defines constants, macros, and declares functions for controlling
 * specific instances of SoundGenChannel and SoundGenVoice objects and their
 * underlying hardware.
 */

#ifndef SOUNDGENHW_H
#define	SOUNDGENHW_H

#include "stdint.h"
#include "stdbool.h"
#include "sfrdefs.h"

#include "SoundGenChannel.h"
#include "SoundGenVoice.h"

typedef enum Channel
{
    SG_CHANNEL_0,
    SG_CHANNEL_1,
    SG_N_CHANNELS
} Channel;


#define SG_CHAN_DUTY_BASE           32
#define SG_CHAN_DUTY_MIN            1
#define SG_CHAN_DUTY_MAX            (SG_CHAN_DUTY_BASE / 2)

#define SG_CHAN_WAVE_RATE           1000000

#define SG_CHAN0_PERIOD_SET(us)  CCPR1H  = us >> 8; \
                                 CCPR1L  = us & 0xFF

#define SG_CHAN0_PERIOD_MATCH_TEST()   (PIR1 & _PIR1_CCP1IF_MASK)
#define SG_CHAN0_PERIOD_MATCH_CLEAR()  PIR1 &= ~_PIR1_CCP1IF_MASK


#define SG_CHAN1_PERIOD_SET(us)  CCPR2H  = us >> 8; \
                                 CCPR2L  = us & 0xFF

#define SG_CHAN1_PERIOD_MATCH_TEST()   (PIR2 & _PIR2_CCP2IF_MASK)
#define SG_CHAN1_PERIOD_MATCH_CLEAR()  PIR2 &= ~_PIR2_CCP2IF_MASK


#define SG_VOICE_TIMER_COUNT (65536 - 125)   /* Timer overflows every 1 mS. */

#define SG_VOICE_TIMER_OVERFLOW_TEST()  (INTCON & _INTCON_TMR0IF_MASK)
#define SG_VOICE_TIMER_OVERFLOW_CLEAR() INTCON &= ~_INTCON_TMR0IF_MASK

#define SG_VOICE_TIMER_COUNT_RELOAD()   TMR0H = SG_VOICE_TIMER_COUNT >> 8;   \
                                        TMR0L = SG_VOICE_TIMER_COUNT & 0xFF

void SoundGenHwInit     (void);
void SoundGenHwUpdate   (void);

void SoundGenHwVoiceUpdate(void);

ChannelCtlBits * SoundGenHwGetChannelCtlPtr(Channel thisChannel);
VoiceProperties * SoundGenHwGetVoicePtr(Channel thisChannel);

#endif	/* SOUNDGENHW_H */

