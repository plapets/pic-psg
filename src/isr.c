
#include "isr.h"

/* SPI handling (SSP, RB pin change) */
#ifdef __SDCC
void isr_high_priority(void) __interrupt 1 /* 1 = high priority interrupt    */
#elif __XC8
void __high_priority __interrupt isr_high_priority(void)
#endif
{
    SpiHwSlaveUpdate();
}


/* Envelope and wave timers (Timer0, CCP1, CCP2) */
#ifdef __SDCC
void isr_low_priority(void) __interrupt 2 /* 2 = low priority interrupt */
#elif __XC8
void __low_priority __interrupt isr_low_priority(void)
#endif
{
    SoundGenHwUpdate();
}
