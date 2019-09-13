
#include "SoundGenCommand.h"

/* SoundGenCommandDispatch( command * )
 *  Call this function to execute a received command whose data resides at the
 *  provided command pointer.
 * 
 *  N.B. This function only performs sanity checking on note frequency values,
 *       to prevent overflow and division by zero during wave low and high time
 *       calculations.  No other values are checked.  Invalid parameters may
 *       cause memory corruption and erratic behavior.
 */
void SoundGenCommandDispatch(SoundGenCommandData * commandData)
{
    VoiceProperties * voice;
    Note thisNote;
    
    voice = SoundGenHwGetVoicePtr(commandData->fields.channel);
    
    switch (commandData->fields.command)
    {
        case SG_CMD_SET_RAMP:
            SoundGenVoiceSetAttack(voice, (*commandData).SG_CMD_DATA_RAMP);
            break;
        
        case SG_CMD_SET_DUTY:
            SoundGenVoiceSetDuty(voice, (*commandData).SG_CMD_DATA_DUTY);
            break;
        
        case SG_CMD_SET_ATTACK:
            SoundGenVoiceSetTimeAttack(voice, (*commandData).SG_CMD_DATA_ATTACK);
            break;
        
        case SG_CMD_SET_RELEASE:
            SoundGenVoiceSetTimeRelease(voice, (*commandData).SG_CMD_DATA_RELEASE);
            break;
        
        case SG_CMD_PLAY_NOTE:
            thisNote.frequency = (*commandData).SG_CMD_DATA_FREQUENCY;
            thisNote.duration = (*commandData).SG_CMD_DATA_DURATION;
            
            if (thisNote.frequency < 20)
                thisNote.frequency = 20;
            
            SoundGenVoicePlayNote(voice, &thisNote);
            break;
    }
}
