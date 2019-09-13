/* 
 * File:   Spi.h
 * Author: peterl
 *
 * Created on April 2, 2018, 11:13 AM
 * 
 * This file defines an interface for controlling PIC SPI hardware, particularly
 * concerning slave communications.
 */

#ifndef SPI_H
#define	SPI_H

#include "sfrdefs.h"

#include "stdint.h"
#include "stdbool.h"

#define SPI_BUFFER_2N_SIZE (1 << 4)
#define SPI_BUFFER_2N_MASK ((SPI_BUFFER_2N_SIZE) - 1)

#define SPI_SS_PIN_CHANGE_TEST()        (INTCON & _INTCON_RBIF_MASK)
#define SPI_SS_PIN_CHANGE_READ_CLEAR()  (PORTB, INTCON &= ~_INTCON_RBIF_MASK, !(PORTB & 0x10))

#define SPI_TRANSACTION_TEST()          (PIR1 & _PIR1_SSPIF_MASK)
#define SPI_TRANSACTION_CLEAR()         PIR1 &= ~_PIR1_SSPIF_MASK;

typedef enum spi_mode_t
{
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3,
    NUM_SPI_MODES
} spi_mode_t;

typedef union spi_mode_fields_t
{
    struct
    {
        uint8_t CPHA    : 1,
                CPOL    : 1;
    };
    
    spi_mode_t mode;
} spi_mode_fields_t;

void SpiHwMasterInit(spi_mode_t mode);
void SpiHwMasterWrite(uint8_t byte);

void SpiHwSlaveInit(spi_mode_t mode);
void SpiHwSlaveUpdate(void);

bool        SpiHwSlaveDataAvailable(void);

uint8_t *   SpiHwSlaveDataClaim(void);
void        SpiHwSlaveDataRelease(void);
uint8_t     SpiHwSlaveDataSize(void);

#endif	/* SPI_H */
