
#include "SoundGenHw.h"

/* Stores instances of channels mapped to real hardware.    */
static ChannelCtlBits g_SoundGenChannels[SG_N_CHANNELS];

/* Stores instances of voices mapped to real hardware.          */
static VoiceProperties g_SoundGenVoices[SG_N_CHANNELS];

/* Flag triggers periodic execution of SoundGenHwVoiceUpdate(). */
static bool g_DoVoiceUpdate;

static void SoundGenHwInitWaveTimer0(void);
static void SoundGenHwInitWaveTimer1(void);
static void SoundGenHwInitVoiceTimer(void);

/* SoundGenHwInitWaveTimer0()
 *  This is a helper function called internally by SoundGenHwInit() to
 *  initialize the Timer1 and CCP1 hardware for channel 0 wave generation.
 */
static void SoundGenHwInitWaveTimer0(void)
{
    PIE1   &=  ~_PIE1_TMR1IE_MASK;          /* Disable timer1 interrupt.    */
    PIR1   &=  ~_PIR1_TMR1IF_MASK;          /* Clear timer1 interrupt flag. */
    
    PIE1   &=  ~_PIE1_CCP1IE_MASK;          /* Disable compare interrupt.   */
    
    T1CON   =   _T1CON_RD16_MASK    |       /* Enable 16-bit reads/writes.  */
                _T1CON_T1CKPS1_MASK |       /* Set prescaler to 1:8.        */
                _T1CON_T1CKPS0_MASK ;       /*  ...                         */
    
    IPR1   &=  ~_IPR1_TMR1IP_MASK;          /* Timer priority is low.       */

    TMR1H   =   0;                          /* Clear the count register.    */
    TMR1L   =   0;
    
    CCP1CON =   _CCP1CON_CCP1M3_MASK |      /* Set compare mode, trigger    */
                _CCP1CON_CCP1M1_MASK |      /*  special event, reset timer1.*/
                _CCP1CON_CCP1M0_MASK ;      /*  ...                         */
    
    IPR1   &=  ~_IPR1_CCP1IP_MASK;          /* Compare priority is low.     */
    
    SG_CHAN0_PERIOD_SET(0xFFFF);
    
    PIR1   &=  ~_PIR1_CCP1IF_MASK;          /* Clear compare interrupt flag.*/
    
    PIE1   |=   _PIE1_CCP1IE_MASK;          /* Enable compare interrupt.    */
    T1CON  |=   _T1CON_TMR1ON_MASK;         /* Enable timer1.               */
}

/* SoundGenHwInitWaveTimer1()
 *  This is a helper function called internally by SoundGenHwInit() to
 *  initialize the Timer3 and CCP2 hardware for channel 0 wave generation.
 */
static void SoundGenHwInitWaveTimer1(void)
{
    PIE2   &=  ~_PIE2_TMR3IE_MASK;          /* Disable timer3 interrupt.    */
    PIR2   &=  ~_PIR2_TMR3IF_MASK;          /* Clear timer3 interrupt flag. */
    
    PIE2   &=  ~_PIE2_CCP2IE_MASK;          /* Disable compare interrupt.   */
    
    T3CON   =   _T3CON_RD16_MASK    |       /* Enable 16-bit reads/writes.  */
                _T3CON_T3CKPS1_MASK |       /* Set prescaler to 1:8.        */
                _T3CON_T3CKPS0_MASK |       /*  ...                         */
                _T3CON_T3CCP1_MASK  ;       /* Timer3 controls CCPR2.       */
    
    IPR2   &=  ~_IPR2_TMR3IP_MASK;          /* Timer priority is low.       */

    TMR3H   =   0;                          /* Clear the count register.    */
    TMR3L   =   0;
    
    CCP2CON =   _CCP2CON_CCP2M3_MASK |      /* Set compare mode, trigger    */
                _CCP2CON_CCP2M1_MASK |      /*  special event, reset timer3.*/
                _CCP2CON_CCP2M0_MASK ;      /*  ...                         */
    
    IPR2   &=  ~_IPR2_CCP2IP_MASK;          /* Compare priority is low.     */
    
    SG_CHAN1_PERIOD_SET(0xFFFF);
    
    PIR2   &=  ~_PIR2_CCP2IF_MASK;          /* Clear compare interrupt flag.*/
    
    PIE2   |=   _PIE2_CCP2IE_MASK;          /* Enable compare interrupt.    */
    T3CON  |=   _T3CON_TMR3ON_MASK;         /* Enable timer3.               */
}

/* SoundGenHwInitVoiceTimer()
 *  This is a helper function called internally by SoundGenHwInit() to
 *  initialize the Timer0 hardware.
 * 
 *  N.B. A Timer0 overflow interrupt is used to set g_DoVoiceUpdate and thereby
 *       allow SoundGenHwVoiceUpdate() to execute at appropriate intervals.
 */
static void SoundGenHwInitVoiceTimer(void)
{
    INTCON &=  ~_INTCON_TMR0IE_MASK;        /* Disable timer0 interrupt.    */
    INTCON &=  ~_INTCON_TMR0IF_MASK;        /* Clear timer0 interrupt flag. */
    
                                            /* Timer uses rising edge of    */
                                            /*  the internal clock.         */
                                            /* Setup timer in 16-bit mode.  */
    T0CON   =   _T0CON_T0PS0_MASK       |   /* Set prescaler to 1:64.       */
                _T0CON_T0PS2_MASK       ;   /*  ...                         */
    
    SG_VOICE_TIMER_COUNT_RELOAD();          /* Load timer count value.      */
    
    INTCON |=   _INTCON_TMR0IE_MASK;        /* Enable timer0 interrupt.     */
    T0CON  |=   _T0CON_TMR0ON_MASK;         /* Enable timer0.               */
}

/* SoundGenHwInit()
 *  Call this function to associate channel and voice instances with available
 *  hardware and initialize them to default values.
 */
void SoundGenHwInit(void)
{
    SoundGenChannelInit(&g_SoundGenChannels[SG_CHANNEL_0], &LATA, &TRISA);
    SoundGenChannelInit(&g_SoundGenChannels[SG_CHANNEL_1], &LATB, &TRISB);
    
    SoundGenVoiceInit(&g_SoundGenVoices[SG_CHANNEL_0], &g_SoundGenChannels[SG_CHANNEL_0]);
    SoundGenVoiceInit(&g_SoundGenVoices[SG_CHANNEL_1], &g_SoundGenChannels[SG_CHANNEL_1]);
    
    SoundGenHwInitWaveTimer0();
    SoundGenHwInitWaveTimer1();
    
    SoundGenHwInitVoiceTimer();
}

/* SoundGenHwUpdate()
 *  This function is called from a low-priority interrupt in one of two cases:
 * 
 *  1. A timer compare match has occurred.  This function will toggle the level
 *     of the wave output pin associated with the timer's respective channel,
 *     and reload the compare match register with the new high or low period.
 *  2. A timer overflow has occurred.  The function will set g_DoVoiceUpdate to
 *     schedule execution of voice update code in main().
 */
void SoundGenHwUpdate(void)
{
    if (SG_CHAN0_PERIOD_MATCH_TEST())
    {
        SG_CHAN0_PERIOD_MATCH_CLEAR();
        
        SoundGenChannelWaveToggle(&g_SoundGenChannels[SG_CHANNEL_0]);
        
        if (SoundGenChannelWavePhase(&g_SoundGenChannels[SG_CHANNEL_0]) == SG_CHAN_CTL_WAVE_HIGH)
        {
            SG_CHAN0_PERIOD_SET(SoundGenChannelWaveGetTimeHigh(&g_SoundGenChannels[SG_CHANNEL_0]));
        }
        else
        {
            SG_CHAN0_PERIOD_SET(SoundGenChannelWaveGetTimeLow(&g_SoundGenChannels[SG_CHANNEL_0]));
        }
    }
    
    if (SG_CHAN1_PERIOD_MATCH_TEST())
    {
        SG_CHAN1_PERIOD_MATCH_CLEAR();
        
        SoundGenChannelWaveToggle(&g_SoundGenChannels[SG_CHANNEL_1]);
        
        if (SoundGenChannelWavePhase(&g_SoundGenChannels[SG_CHANNEL_1]) == SG_CHAN_CTL_WAVE_HIGH)
        {
            SG_CHAN1_PERIOD_SET(SoundGenChannelWaveGetTimeHigh(&g_SoundGenChannels[SG_CHANNEL_1]));
        }
        else
        {
            SG_CHAN1_PERIOD_SET(SoundGenChannelWaveGetTimeLow(&g_SoundGenChannels[SG_CHANNEL_1]));
        }
    }
    
    if (SG_VOICE_TIMER_OVERFLOW_TEST())
    {
        SG_VOICE_TIMER_COUNT_RELOAD();
        SG_VOICE_TIMER_OVERFLOW_CLEAR();
        
        g_DoVoiceUpdate = true;
    }
}

/* SoundGenHwVoiceUpdate()
 *  Call this function regularly from a non-ISR routine to update the envelope
 *  generation hardware.  On Timer0 overflow, g_DoVoiceUpdate is set and the
 *  function body executes.
 */
void SoundGenHwVoiceUpdate(void)
{
    if (g_DoVoiceUpdate)
    {
        g_DoVoiceUpdate = false;
        SoundGenVoiceUpdate(&g_SoundGenVoices[SG_CHANNEL_0]);
        SoundGenVoiceUpdate(&g_SoundGenVoices[SG_CHANNEL_1]);
    }
}

/* SoundGenHwGetChannelCtlPtr( channelnumber )
 *  This function returns a pointer to one of the two available hardware-mapped
 *  channel data structures.
 */
ChannelCtlBits * SoundGenHwGetChannelCtlPtr(Channel thisChannel)
{
    return (&g_SoundGenChannels[thisChannel]);
}

/* SoundGenHwGetVoicePtr( channelnumber )
 *  This function returns a pointer to one of the two available hardware-mapped
 *  voice data structures.
 */
VoiceProperties * SoundGenHwGetVoicePtr(Channel thisChannel)
{
    return (&g_SoundGenVoices[thisChannel]);
}

