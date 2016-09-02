/* 
 * File:   i2c.h
 * Author: AlbertSanchez
 * Micro:  PIC12F1822
 */

#ifndef I2C_H
#define	I2C_H

#include <xc.h> 
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define START_BIT       SSP1STATbits.S
#define STOP_BIT        SSP1STATbits.P

#define CKP             SSP1CON1bits.CKP
#define W_COLLISION     SSP1CON1bits.WCOL
#define ACKSTAT         SSP1CON2bits.ACKSTAT

#define BUFFER          SSP1BUF  
#define BUFFER_FLAG     SSP1STATbits.BF

#define I2C_INTERRUPT   PIR1bits.SSP1IF
#define R_nW            SSP1STATbits.R_nW 
#define D_nA            SSP1STATbits.D_nA

#define READ            1
#define WRITE           0

#define VOLTAGE_MEAS    0x0A
#define CURRENT_MEAS    0x0B
#define POL_STATE       0x0C
#define DISABLE         0x0D
#define ENABLE          0x0E


/**
    @Description
        This routine initializes the i2c slave mode

    @Preconditions
        None

    @Param
        None

    @Returns
        None
*/
void I2C_Init(void);

#endif	/* I2C_H */

