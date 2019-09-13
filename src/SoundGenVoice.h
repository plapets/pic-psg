/* 
 * File:   SoundGenVoiceCtl.h
 * Author: peterl
 *
 * Created on April 17, 2018, 1:34 PM
 * 
 * This file defines VoiceProperties, a stateful abstraction of ChannelCtlBits
 * which allows for persistent channel-related settings such as envelope ramp
 * speed, duty cycle, and attack and release times for played notes.
 */

#ifndef SOUNDGENVOICECTL_H
#define	SOUNDGENVOICECTL_H

#include "SoundGenChannel.h"

typedef enum VoiceAttack
{
    SG_VOICE_ATTACK_SLOW,
    SG_VOICE_ATTACK_MEDIUM,
    SG_VOICE_ATTACK_FAST,
    SG_VOICE_N_ATTACKS
} VoiceAttack;

typedef enum VoiceAction
{
    SG_VOICE_ACTION_IDLE,
    SG_VOICE_ACTION_START,
    SG_VOICE_ACTION_ATTACK,
    SG_VOICE_ACTION_SUSTAIN,
    SG_VOICE_ACTION_RELEASE
} VoiceAction;

typedef struct Note
{
    uint16_t frequency;
    uint8_t duration;
} Note;

typedef uint16_t ticks_t;

typedef volatile struct VoiceProperties
{
    /* Inherited Properties */
    ChannelCtlBits * volatile portState;
    
    /* Voice Timbre         */
    VoiceAttack attack;         /* Abstraction of ramp speed.               */
    uint8_t     duty;           /* Abstraction of timeHigh and timeLow.     */
    
    ticks_t voiceTimeAttack;    /* Time to increase note amplitude.         */
    ticks_t voiceTimeRelease;   /* Time to decrease note amplitude.         */
    
    /* Current Note         */
    VoiceAction action;         /* Holds the current envelope state.        */
    
    /* These fields hold the maximum allowable envelope durations for the
     * currently playing note.  If a note duration is shorter than the sum of
     * the set voice attack, sustain, and release durations, the attack and
     * sustain times of the played note will be shortened.
     */
    ticks_t noteTimeAttack;     /* Ticks remaining in the current attack.   */
    ticks_t noteTimeSustain;    /* Ticks remaining in the current sustain.  */
    ticks_t noteTimeRelease;    /* Ticks remaining in the current release.  */
} VoiceProperties;

#include "SoundGenHw.h"

void SoundGenVoiceInit  (VoiceProperties * voice, ChannelCtlBits * channel);

void SoundGenVoiceSetAttack     (VoiceProperties * voice, VoiceAttack attack);
void SoundGenVoiceSetDuty       (VoiceProperties * voice, uint8_t duty);

void SoundGenVoiceSetTimeAttack (VoiceProperties * voice, ticks_t timeAttack);
void SoundGenVoiceSetTimeRelease(VoiceProperties * voice, ticks_t timeRelease);

void SoundGenVoicePlayNote      (VoiceProperties * voice, Note * note );

void SoundGenVoiceUpdate        (VoiceProperties * voice);

#endif	/* SOUNDGENVOICECTL_H */

