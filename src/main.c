/*
 * File:   main.c
 * Author: peterl
 * 
 * Created on February 4, 2018, 8:45 PM
 */


/* System includes  */
#include "stdint.h"
#include "stdbool.h"

#include "sfrdefs.h"
#include "fuses.h"
#include "isr.h"

#include "InitHw.h"

/* App includes */
#include "Spi.h"
#include "SoundGenHw.h"
#include "SoundGenCommand.h"

void init(void);    /* Root initialization procedure.   */
void loop(void);    /* Main procedure; infinite loop.   */

void main(void)
{
    init();
    loop();
    
    return;
}

void init(void)
{
    di();                       /* Disable int.'s during setup.     */
    
    InitHwMinimal();            /* Do minimal configuration.        */
    SpiHwSlaveInit(SPI_MODE_0); /* Setup SPI slave mode.            */
    SoundGenHwInit();           /* Initialize the sound hardware.   */
    
    ei();                       /* Re-enable interrupts.            */
}

void loop(void)
{
	while (1)
	{
        if (SpiHwSlaveDataAvailable())
        {
            SoundGenCommandDispatch((SoundGenCommandData *) SpiHwSlaveDataClaim());
            
            SpiHwSlaveDataRelease();    /* Allow SPI buffer writes. */
        }
        
        SoundGenHwVoiceUpdate();
	}
}
