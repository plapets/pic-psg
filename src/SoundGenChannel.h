/* 
 * File:   SoundGenChannelCtl.h
 * Author: peterl
 *
 * Created on April 17, 2018, 12:04 PM
 * 
 * This file defines an interface for directly controlling wave- and envelope-
 * generation-related hardware.
 */

#ifndef SOUNDGENCHANNELCTL_H
#define	SOUNDGENCHANNELCTL_H

#include "stdint.h"

#define SG_CHAN_CTL_RAMP_DOWN       0
#define SG_CHAN_CTL_RAMP_UP        ~0U

#define SG_CHAN_CTL_RAMP_HOLD       0x07    /* 111 */
#define SG_CHAN_CTL_RAMP_SLOW       0x06    /* 110 */
#define SG_CHAN_CTL_RAMP_MEDIUM     0x05    /* 101 */
#define SG_CHAN_CTL_RAMP_FAST       0x03    /* 011 */

#define SG_CHAN_CTL_WAVE_UNMUTED    0
#define SG_CHAN_CTL_WAVE_MUTED      1

#define SG_CHAN_CTL_WAVE_LOW        0
#define SG_CHAN_CTL_WAVE_HIGH       1

typedef volatile struct ChannelCtlBits
{
    struct ChannelCtlBits_Latch
    {
        uint8_t rampDirection   : 3,
                wavePhase       : 1;
    } * latch;  /* points to LATx.  */
    
    struct ChannelCtlBits_State
    {
        uint8_t rampState       : 3,    /* A low bit selects one of three   */
                                        /*  envelope slopes.                */
                waveState       : 1;    /* Muted or unmuted.                */
    } * state;  /* points to TRISx. */
    
    uint16_t waveTimeLow;   /* On timer compare match, one of these values  */
    uint16_t waveTimeHigh;  /*  is selected to reload the compare match     */
                            /*  register, depending on wavePhase.           */
} ChannelCtlBits;

void    SoundGenChannelInit         (   ChannelCtlBits *    channel,
                                        volatile uint8_t *  portLatch,
                                        volatile uint8_t *  portState   );

void    SoundGenChannelWaveMute     (   ChannelCtlBits * channel    );
void    SoundGenChannelWaveUnmute   (   ChannelCtlBits * channel    );
void    SoundGenChannelWaveToggle   (   ChannelCtlBits * channel    );
uint8_t SoundGenChannelWavePhase    (   ChannelCtlBits * channel    );

void        SoundGenChannelWaveSetTimeLow   ( ChannelCtlBits * channel, uint16_t timeLow    );
void        SoundGenChannelWaveSetTimeHigh  ( ChannelCtlBits * channel, uint16_t timeHigh   );
uint16_t    SoundGenChannelWaveGetTimeLow   ( ChannelCtlBits * channel);
uint16_t    SoundGenChannelWaveGetTimeHigh  ( ChannelCtlBits * channel);

void    SoundGenChannelRampSetDirection ( ChannelCtlBits * channel, uint8_t direction );
void    SoundGenChannelRampSetSpeed     ( ChannelCtlBits * channel, uint8_t speed );

#endif	/* SOUNDGENCHANNELCTL_H */
