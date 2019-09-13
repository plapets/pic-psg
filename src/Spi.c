
#include "Spi.h"

static uint8_t  g_SpiDataBuffer[SPI_BUFFER_2N_SIZE];    /* SPI slave data receive buffer.           */
static uint8_t  g_SpiDataBufferIndex = 0;               /* Stores the number of bytes received after*/
                                                        /*  a transaction is completed.             */
static bool     g_SpiDataAvailable          = false;    /* The buffer has been filled with new data.*/
static bool     g_SpiDataBufferWriteAllowed = true;     /* The buffer is not in use by application. */

/* SpiHwMasterInit( mode )
 *  This function initializes the SPI hardware as master in one of the four
 *  modes.  The SPI clock rate is hard-coded.
 */
void SpiHwMasterInit(spi_mode_t mode)
{
    spi_mode_fields_t spi_mode_fields;
    spi_mode_fields.mode = mode;
    
    SSPCON1 &= ~_SSPCON1_SSPEN_MASK;    /* Clear the SSPEN bit.             */
    
    TRISC   &= ~0x28;                   /* Clear TRISC<3> for master SCK.   */
                                        /* Clear TRISC<5> for SDO.          */
    TRISC   |=  0x10;                   /* This pin is jumpered to RC5.     */
    
    SSPSTAT = (SSPSTAT & ~_SSPSTAT_CKE_MASK) | ((~spi_mode_fields.CPHA) << _SSPSTAT_CKE_POSN);
    SSPCON1 = (spi_mode_fields.CPOL << _SSPCON1_CKP_POSN) | 0x01;
    
    SSPCON1 |= _SSPCON1_SSPEN_MASK;     /* Set the SSPEN bit.               */
    
    SSPBUF = 0;
    PIR1bits.SSPIF = 0;
}

/* SpiHwMasterWrite( byte )
 *  This function queues a byte for transmission.
 * 
 *  N.B. The calling code is responsible for any chip select operations.
 */
void SpiHwMasterWrite(uint8_t byte)
{
    PIR1bits.SSPIF = 0;
    SSPBUF = byte;
    while (PIR1bits.SSPIF == 0);
}

/* SpiHwSlaveInit( mode )
 *  This function initializes the SPI hardware as slave in one of the four SPI
 *  modes.
 *  
 *  N.B. This and other slave-related functions assume the following:
 * 
 *  1. RB4 is tied to !SS.
 *  2. RB<4:7> pins are NOT configured as analog inputs, and RB<5:7> are unused
 *     by other circuitry.
 *  3. SpiHwSlaveUpdate() is invoked immediately by the high-priority ISR.
 * 
 * This configuration expects that RB<4> will invoke a pin change interrupt. The
 * function sets pins RB<4:7> as inputs and engages the internal pull-up for
 * RB<5:7> to prevent spurious interrupts from any noise present due to their
 * high-Z state.  A consequence of the fact that these pins are all connected to
 * a single interrupt line is that they cannot be used in this configuration.
 */
void SpiHwSlaveInit(spi_mode_t mode)
{
    spi_mode_fields_t spi_mode_fields;
    spi_mode_fields.mode = mode;
    
    INTCON  &= ~_INTCON_RBIE_MASK;      /* Disable PORTB int. on pin change.*/
    TRISB   |=  0xF0;                   /* Set RB<4:7> as inputs.           */
    
    LATB     =  (LATB & ~0x10) | 0xE0;  /* Enable pull-ups on RB<5:7>.      */
    INTCON2 &= ~_INTCON2_NOT_RBPU_MASK; /*  ...                             */
    LATB     =  PORTB;                  /* End possible mismatch condition. */
    INTCON  &= ~_INTCON_RBIF_MASK;      /* Clear interrupt flag.            */
    INTCON2 |=  _INTCON2_RBIP_MASK;     /* Pin change int. is high priority.*/
    INTCON  |=  _INTCON_RBIE_MASK;      /* Enable PORTB int. on pin change. */
    
    
    PIE1 &= ~_PIE1_SSPIE_MASK;          /* Disable the SPI interrupt.       */
    PIR1 &= ~_PIR1_SSPIF_MASK;          /* Clear the interrupt flag.        */
    IPR1 |=  _IPR1_SSPIP_MASK;          /* Use high-priority interrupt.     */
    
    SSPCON1 &= ~_SSPCON1_SSPEN_MASK;    /* Clear the SSPEN bit.             */
    
    TRISA   |=  0x20;                   /* Set TRISA<5> for ~SS and SDI.    */
    LATA    |=  0x20;                   /* Data latch must be high for SPI  */
                                        /*  slave operation.                */
    
    TRISC   |=  0x18;                   /* Set TRISC<3> for slave SCK.      */
    TRISC   &= ~0x20;                   /* Clear TRISC<5> for SDO.          */
    
    SSPSTAT = (SSPSTAT & ~_SSPSTAT_CKE_MASK) | ((~spi_mode_fields.CPHA) << _SSPSTAT_CKE_POSN);
    SSPCON1 = (spi_mode_fields.CPOL << _SSPCON1_CKP_POSN) | 0x04;
    
    SSPCON1 |= _SSPCON1_SSPEN_MASK;     /* Set the SSPEN bit.               */
    PIE1    |= _PIE1_SSPIE_MASK;        /* Enable the interrupt.            */
}

/* SpiHwSlaveUpdate()
 *  This function reads data from the SPI hardware and places it into
 *  g_SpiDataBuffer[].  At the end of the data reception, the number of bytes
 *  received is present in g_SpiDataBufferIndex.  The buffer and reported data
 *  length will only change if g_SpiDataBufferWriteAllowed is true.  This allows
 *  an application to claim access of the data buffer in the midst of an
 *  operation.
 * 
 *  N.B. This function should be invoked from the high-priority ISR.
 */
void SpiHwSlaveUpdate(void)
{
    static bool bufferWriteAllowed = true;
    
    /* Service SPI first and return.  This way, even if ~SS becomes high before
     * the SPI buffer is filled with the last byte, ~SS will be serviced
     * on the next interrupt instead.
     */
    if (SPI_TRANSACTION_TEST())
    {
        SPI_TRANSACTION_CLEAR();
        
        if (bufferWriteAllowed)
        {
            g_SpiDataBuffer[g_SpiDataBufferIndex++] = SSPBUF;
            g_SpiDataBufferIndex &= SPI_BUFFER_2N_MASK;
        }
        else
        {
            SSPBUF;
        }
        
        return; /* Ensure last data read occurs before ~SS is serviced. */
    }
    
    /* If the ~SS pin changes state */
    if (SPI_SS_PIN_CHANGE_TEST())
    {
        if (SPI_SS_PIN_CHANGE_READ_CLEAR())
        {
            /* If ~SS is brought low, record if the app has ownership of the
             * buffer.  If not, reset the buffer and allow writing. 
             */
            bufferWriteAllowed = g_SpiDataBufferWriteAllowed;
            g_SpiDataBufferIndex = (bufferWriteAllowed) ? 0 : g_SpiDataBufferIndex;
        }
        else
        {
            /* If ~SS is brought high, and if data was written to the buffer,
             * let the app know that there is fresh data.
             */
            g_SpiDataAvailable |= bufferWriteAllowed;
        }
    }
}

/* SpiHwSlaveDataAvailable()
 *  This function returns true if data has been received since a prior call to
 *  SpiHwSlaveDataClaim().
 * 
 *  N.B. This function only reports the availability of data.  If the data in
 *       the buffer is not claimed, it may be overwritten by incoming data.
 */
bool SpiHwSlaveDataAvailable(void)
{
    return (g_SpiDataAvailable);
}

/* SpiHwSlaveDataClaim()
 *  This function returns a pointer to the data received in the last SPI
 *  transaction.  A call to this function ensures that data found at the pointer
 *  will not be overwritten by subsequent incoming data until a call is made to
 *  SpiHwSlaveDataRelease().  In addition, it clears g_SpiDataAvailable, which
 *  allows for the application to wait or perform other tasks until new data has
 *  been placed into the buffer.
 */
uint8_t * SpiHwSlaveDataClaim(void)
{
    g_SpiDataAvailable = false;
    g_SpiDataBufferWriteAllowed = false;
    return (&g_SpiDataBuffer[0]);
}

/* SpiHwSlaveDataRelease()
 *  This function allows the SPI receive buffer data to be overwritten by
 *  subsequent incoming data.
 */
void SpiHwSlaveDataRelease(void)
{
    g_SpiDataBufferWriteAllowed = true;
}

/* SpiHwSlaveDataSize()
 *  This function returns the size in bytes of the last received data.
 * 
 *  N.B. This function only reports the size of available data.  If the data in
 *       the buffer is not claimed, the reported size may be invalid.
 */
uint8_t SpiHwSlaveDataSize(void)
{
    return (g_SpiDataBufferIndex);
}
