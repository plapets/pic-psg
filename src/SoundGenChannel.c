
#include "SoundGenChannel.h"

/* SoundGenChannelInit( channel *, latch *, state * )
 *  Call this function to map a ChannelCtlBits structure to a hardware port and
 *  initialize its fields.  On exit, the channel is set to ramp down at the
 *  steepest slope, to ensure initial discharge of the envelope circuitry.
 */
void SoundGenChannelInit(   ChannelCtlBits *    channel,
                            volatile uint8_t *  portLatch,
                            volatile uint8_t *  portState   )
{
    channel->latch = (struct ChannelCtlBits_Latch *) portLatch;
    channel->state = (struct ChannelCtlBits_State *) portState;
    
    SoundGenChannelWaveMute(channel);
    SoundGenChannelWaveSetTimeHigh(channel, 0x1000);
    SoundGenChannelWaveSetTimeLow(channel, 0x1000);
    
    SoundGenChannelRampSetSpeed(channel, SG_CHAN_CTL_RAMP_FAST);
    SoundGenChannelRampSetDirection(channel, SG_CHAN_CTL_RAMP_DOWN);
}

/* SoundGenChannelWaveMute( channel * )
 *  Call this function to mute the wave output of a channel.  This sets the
 *  pin designated for wave output to a high-impedance state.
 */
void SoundGenChannelWaveMute    (ChannelCtlBits * channel)
{
    channel->state->waveState = SG_CHAN_CTL_WAVE_MUTED;
}

/* SoundGenChannelWaveUnmute( channel * )
 *  Call this function to unmute the wave output of a channel.  This sets the
 *  pin designated for wave output to an output state.
 */
void SoundGenChannelWaveUnmute  (ChannelCtlBits * channel)
{
    channel->state->waveState = SG_CHAN_CTL_WAVE_UNMUTED;
}

/* SoundGenChannelWaveToggle( channel * )
 *  This function toggles the level of a channel's wave output pin.
 * 
 *  N.B. This function is used internally to generate the output waveform on a
 *       timer compare match.
 */
void SoundGenChannelWaveToggle  (ChannelCtlBits * channel)
{
    channel->latch->wavePhase ^= 1;
}

/* SoundGenChannelWavePhase( channel * )
 *  This function returns the current level of a channel's wave output pin.
 * 
 *  N.B. This function is used internally, in conjunction with a channel's
 *       waveTimeLow and waveTimeHigh fields, to load the appropriate timer
 *       compare match value for wave generation.
 */
uint8_t SoundGenChannelWavePhase (ChannelCtlBits * channel)
{
    return (channel->latch->wavePhase);
}

/* SoundGenChannelWaveSetTimeLow( channel *, timeLow )
 *  This function sets the low time of a channel's output wave in microseconds.
 *  The stored value is loaded into a timer compare match register every half-
 *  cycle of the output waveform.
 */
void SoundGenChannelWaveSetTimeLow(ChannelCtlBits * channel, uint16_t timeLow)
{
    channel->waveTimeLow = timeLow;
}

/* SoundGenChannelWaveSetTimeHigh( channel *, timeHigh )
 *  This function sets the high time of a channel's output wave in microseconds.
 *  The stored value is loaded into a timer compare match register every half-
 *  cycle of the output waveform.
 */
void SoundGenChannelWaveSetTimeHigh(ChannelCtlBits * channel, uint16_t timeHigh)
{
    channel->waveTimeHigh = timeHigh;
}

/* SoundGenChannelWaveGetTimeLow( channel * )
 *  This function returns the low time of a channel's output wave, as set
 *  previously by SoundGenChannelWaveSetTimeLow().
 * 
 *  N.B. This function is used internally, in conjunction with a channel's
 *       wavePhase field, to load the appropriate timer compare match value
 *       for wave generation.
 */
uint16_t SoundGenChannelWaveGetTimeLow(ChannelCtlBits * channel)
{
    return (channel->waveTimeLow);
}

/* SoundGenChannelWaveGetTimeHigh( channel * )
 *  This function returns the high time of a channel's output wave, as set
 *  previously by SoundGenChannelWaveSetTimeHigh().
 * 
 *  N.B. This function is used internally, in conjunction with a channel's
 *       wavePhase field, to load the appropriate timer compare match value
 *       for wave generation.
 */
uint16_t SoundGenChannelWaveGetTimeHigh(ChannelCtlBits * channel)
{
    return (channel->waveTimeHigh);
}

/* SoundGenChannelRampSetDirection( channel *, direction )
 *  Call this function to set the direction of the envelope ramp.  This function
 *  sets the levels of a channel's envelope control pins.
 * 
 *  N.B. This function only sets the ramp direction.  The ramp must be engaged
 *       by calling SoundGenChannelRampSetSpeed() with any ramp speed except
 *       SG_CHAN_CTL_RAMP_HOLD to manifest the effect of this function.  This
 *       function may not yield meaningful results if called with values other
 *       than the two defined ramp direction constants.
 */
void SoundGenChannelRampSetDirection ( ChannelCtlBits * channel, uint8_t direction )
{
    channel->latch->rampDirection = direction;
}

/* SoundGenChannelRampSetSpeed( channel *, speed )
 *  Call this function to set a channel's envelope ramp speed.  This function
 *  sets the state of a channel's envelope control pins.
 * 
 *  N.B. This function only sets the ramp speed.  The ramp direction must also
 *       be set.  The recommended procedure to start an envelope is:
 * 
 *  1. Set the ramp speed to SG_CHAN_CTL_RAMP_HOLD.
 *  2. Set the ramp direction as appropriate.
 *  3. Engage the ramp by setting the desired speed.
 * 
 *      This function may not yield meaningful results if called with values
 *      other than the four defined ramp speed constants.
 */
void SoundGenChannelRampSetSpeed ( ChannelCtlBits * channel, uint8_t speed )
{
    channel->state->rampState = speed;
}
