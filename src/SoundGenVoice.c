
#include "SoundGenVoice.h"

/* Indexing with VoiceAttack returns the bit values for ChannelCtlBits rampState.   */
static const uint8_t g_AttackRampMap[SG_VOICE_N_ATTACKS] = {    SG_CHAN_CTL_RAMP_SLOW,
                                                                SG_CHAN_CTL_RAMP_MEDIUM,
                                                                SG_CHAN_CTL_RAMP_FAST   };

/* SoundGenVoiceInit( voice * )
 *  Call this function to assign a ChannelCtlBits structure to a voice and
 *  initialize its fields.
 */
void SoundGenVoiceInit(VoiceProperties * voice, ChannelCtlBits * channel)
{
    voice->portState = channel;
    
    SoundGenVoiceSetAttack(voice, SG_VOICE_ATTACK_FAST);
    SoundGenVoiceSetDuty(voice, 16);
    
    SoundGenVoiceSetTimeAttack(voice, 50);
    SoundGenVoiceSetTimeRelease(voice, 60);
    
    voice->action = SG_VOICE_ACTION_IDLE;
}

/* SoundGenVoiceSetAttack( voice *, ramp speed )
 *  Call this function to set a voice's ramp speed.  When playing notes, this
 *  value will be used to set the hardware channel's ramp speed during note
 *  attack and release times.
 */
void SoundGenVoiceSetAttack(VoiceProperties * voice, VoiceAttack attack)
{
    voice->attack = attack;
}

/* SoundGenVoiceSetDuty( voice *, wave duty )
 *  Call this function to set a voice's waveform duty cycle.  On the next note,
 *  the field set by this function will be used in conjunction with the note
 *  frequency to set the corresponding channel's high and low time.
 */
void SoundGenVoiceSetDuty(VoiceProperties * voice, uint8_t duty)
{
    voice->duty = duty;
}

/* SoundGenVoiceSetTimeAttack( voice *, attack time )
 *  This function sets the amount of time in milliseconds that this voice should
 *  spend in the attack state when playing a note (i.e. the duration that the
 *  associated channel's ramp bits will be set to a non-hold state in the up
 *  direction).
 */
void SoundGenVoiceSetTimeAttack(VoiceProperties * voice, ticks_t timeAttack)
{
    voice->voiceTimeAttack = timeAttack;
}

/* SoundGenVoiceSetTimeRelease( voice *, release time )
 *  This function sets the amount of time in milliseconds that this voice should
 *  spend in the release state when playing a note (i.e. the duration that the
 *  associated channel's ramp bits will be set to a non-hold state in the down
 *  direction).
 * 
 *  N.B. It is wise to set the release time slightly longer than the attack, in
 *       order to ensure the complete discharge of the envelope between note
 *       renditions.
 */
void SoundGenVoiceSetTimeRelease(VoiceProperties * voice, ticks_t timeRelease)
{
    voice->voiceTimeRelease = timeRelease;
}

/* SoundGenVoicePlayNote( voice *, note * )
 *  Call this function to play a note on a voice.  This function initializes
 *  stateful fields of a VoiceProperties object which govern the note-playing
 *  behavior of a voice.  Calls to SoundGenVoiceUpdate() will update these
 *  fields and change properties of the hardware channel as necessary.
 */
void SoundGenVoicePlayNote(VoiceProperties * voice, Note * note )
{
    uint16_t notePeriod;
    uint16_t noteDutyHigh;
    uint16_t noteDutyLow;
    
    ticks_t noteTimeRemaining;
    
    /* Calculate duty cycle.    */
    notePeriod = SG_CHAN_WAVE_RATE / (uint32_t) note->frequency;
    noteDutyHigh = (notePeriod * voice->duty) / SG_CHAN_DUTY_BASE;
    noteDutyLow = notePeriod - noteDutyHigh;
    
    /* Calculate time available for various note states.    */
    noteTimeRemaining = note->duration;
    
    voice->noteTimeRelease = (noteTimeRemaining > voice->voiceTimeRelease) ?
        voice->voiceTimeRelease : noteTimeRemaining;
    noteTimeRemaining -= voice->noteTimeRelease;
    
    voice->noteTimeAttack = (noteTimeRemaining > voice->voiceTimeAttack) ?
        voice->voiceTimeAttack : noteTimeRemaining;
    noteTimeRemaining -= voice ->noteTimeAttack;
    
    voice->noteTimeSustain = noteTimeRemaining;
    
    SoundGenChannelWaveSetTimeHigh(voice->portState, noteDutyHigh);
    SoundGenChannelWaveSetTimeLow(voice->portState, noteDutyLow);
    SoundGenChannelWaveUnmute(voice->portState);
    
    voice->action = (voice->noteTimeAttack > 0) ? SG_VOICE_ACTION_START : SG_VOICE_ACTION_IDLE;
}

/* SoundGenVoiceUpdate( voice * )
 *  This function examines and updates a voice's stateful fields to trigger
 *  envelope changes and waveform mute/unmuting for note playback.
 */
void SoundGenVoiceUpdate(VoiceProperties * voice)
{
    switch (voice->action)
    {
        case SG_VOICE_ACTION_IDLE:
            break;
        
        case SG_VOICE_ACTION_START:
            SoundGenChannelRampSetDirection(voice->portState, SG_CHAN_CTL_RAMP_UP);
            SoundGenChannelRampSetSpeed(voice->portState, g_AttackRampMap[voice->attack]);
            
            voice->action = SG_VOICE_ACTION_ATTACK;
            break;
        
        case SG_VOICE_ACTION_ATTACK:
            if (voice->noteTimeAttack > 0)
            {
                voice->noteTimeAttack--;
            }
            else
            {
                SoundGenChannelRampSetSpeed(voice->portState, SG_CHAN_CTL_RAMP_HOLD);
                
                voice->action = SG_VOICE_ACTION_SUSTAIN;
            }
            break;
        
        case SG_VOICE_ACTION_SUSTAIN:
            if (voice->noteTimeSustain > 0)
            {
                voice->noteTimeSustain--;
            }
            else
            {
                SoundGenChannelRampSetDirection(voice->portState, SG_CHAN_CTL_RAMP_DOWN);
                SoundGenChannelRampSetSpeed(voice->portState, g_AttackRampMap[voice->attack]);
                
                voice->action = SG_VOICE_ACTION_RELEASE;
            }
            break;
        
        case SG_VOICE_ACTION_RELEASE:
            if (voice->noteTimeRelease > 0)
            {
                voice->noteTimeRelease--;
            }
            else
            {
                SoundGenChannelWaveMute(voice->portState);
                SoundGenChannelRampSetSpeed(voice->portState, SG_CHAN_CTL_RAMP_HOLD);
                
                voice->action = SG_VOICE_ACTION_IDLE;
            }
            break;
    }
}
