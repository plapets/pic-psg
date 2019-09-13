/* 
 * File:   SoundGenCommand.h
 * Author: peterl
 *
 * Created on April 24, 2018, 8:09 AM
 * 
 * This file defines data structures for a sound generator control command set,
 * its serialization, and its execution.  The exposed command set is a subset of
 * functions defined in the SoundGenVoice module.
 */

#ifndef SOUNDGENCOMMAND_H
#define	SOUNDGENCOMMAND_H

#include "stdint.h"
#include "stdbool.h"

#include "SoundGenHw.h"

typedef union SoundGenCommandData
{
    uint8_t raw[6];
    
    struct
    {
        uint8_t command;
        uint8_t channel;

        union
        {
            struct SetRampAttack
            {
                uint8_t     attack;
                uint8_t     reserved0;
                uint8_t     reserved1;
                uint8_t     reserved2;
            } SetRampAttack;

            struct SetWaveDuty
            {
                uint8_t     duty;
                uint8_t     reserved0;
                uint8_t     reserved1;
                uint8_t     reserved2;
            } SetWaveDuty;

            struct SetTimeAttack
            {
                uint16_t    timeAttack;
                uint8_t     reserved1;
                uint8_t     reserved2;
            } SetTimeAttack;

            struct SetTimeRelease
            {
                uint16_t    timeRelease;
                uint8_t     reserved1;
                uint8_t     reserved2;
            } SetTimeRelease;

            struct PlayNote
            {
                uint16_t    frequency;
                uint16_t     duration;
            } PlayNote;
        } parameters;
    } fields;
} SoundGenCommandData;

#define SG_CMD_DATA_RAMP        fields.parameters.SetRampAttack.attack
#define SG_CMD_DATA_DUTY        fields.parameters.SetWaveDuty.duty
#define SG_CMD_DATA_ATTACK      fields.parameters.SetTimeAttack.timeAttack
#define SG_CMD_DATA_RELEASE     fields.parameters.SetTimeRelease.timeRelease
#define SG_CMD_DATA_FREQUENCY   fields.parameters.PlayNote.frequency
#define SG_CMD_DATA_DURATION    fields.parameters.PlayNote.duration

typedef enum SoundGenCommandSet
{
    SG_CMD_SET_RAMP,
    SG_CMD_SET_DUTY,
    SG_CMD_SET_ATTACK,
    SG_CMD_SET_RELEASE,
    SG_CMD_PLAY_NOTE,
    SG_N_COMMANDS
} SoundGenCommandSet;

void SoundGenCommandDispatch(SoundGenCommandData * commandData);

#endif	/* SOUNDGENCOMMAND_H */

