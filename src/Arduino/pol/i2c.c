/*
 * File:   i2c.c
 * Author: AlbertSanchez
 * Micro:  PIC12F1822
 */

#include <xc.h>
#include "i2c.h"

#define I2C_SLAVE_ADDRESS 0x00 
#define I2C_SLAVE_MASK    0x7F

void I2C_Init(void)
{
    // Initialize the hardware
    
    // R_nW write_noTX; P stopbit_notdetected; S startbit_notdetected; 
    // BF RCinprocess_TXcomplete; SMP High Speed; UA dontupdate; CKE disabled; D_nA lastbyte_address; 
    SSP1STAT = 0x00;
    
    //SDA & SCL PINS sets as inputs
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA1 = 1;
    
    // SSPEN enabled; WCOL no_collision; CKP Idle:Low; 
    // Active:High; SSPM I2CSlave 7bit address; SSPOV no_overflow; 
    SSP1CON1 = 0x26;
    
    // ACKEN disabled; GCEN disabled; PEN disabled; ACKDT acknowledge; 
    // RSEN disabled; RCEN disabled; ACKSTAT received; SEN disabled; 
    SSP1CON2 = 0x00;
    
    // ACKTIM ackseq; SBCDE disabled; BOEN disabled; SCIE disabled; 
    // PCIE disabled; DHEN disabled; SDAHT 100ns; AHEN disabled; 
    SSP1CON3 = 0x00;
    
    // SSPMSK 127; 
    SSP1MSK = (I2C_SLAVE_MASK << 1);  // adjust UI mask for R/nW bit            
    
    // SSPADD 0; 
    SSP1ADD = (I2C_SLAVE_ADDRESS << 1);  // adjust UI address for R/nW bit
}

/*void writeI2C(uint8_t transbyte){
    
    uint8_t collision;
    while(!BUFFER_FLAG);
    while(collision){
        W_COLLISION = 0;
        BUFFER = transbyte;
        if(W_COLLISION)
            collision = 1;
        else
            collision = 0;
    }
    CKP = 1;

}*/


