/*
 * File:   interrupts.c
 * Author: AlbertSanchez
 * Micro:  PIC12F1822
 */

#include <xc.h>
#include "interrupts.h"
#include "adc.h"
#include "i2c.h"

void Interrupts_Init(){
    //Enable global interrupts
    INTCONbits.GIE = 1;
    
    //Enable the peripheral interrupts
    INTCONbits.PEIE = 1;
    
    //Enable the MSSP interrupts
    PIE1bits.SSP1IE = 1;
    
    //Enable the MSSP Bus Collision Interrupt
    //PIE2bits.BCL1IE = 1; 
    
    //Clear pending interrupts
    PIR1bits.SSP1IF = 0;
  
}

void interrupt I2C_MSTR_REQ(void){
    
    uint8_t dataResponse;
    uint16_t adcResponse;
    uint8_t rcvdData;
    uint8_t rcvdAddress;
    
    //check for the interrupt
    if(I2C_INTERRUPT == 1 && R_nW == 0)
    {
        //we have received an interrupt and it matches the device address
        //Master have send a command 
        
        //Interrupt bit cleared
        I2C_INTERRUPT = 0;

        //SSP1CON2bits.ACKDT = 1 (done by hardware);
        // Read the rcvdAddress
        rcvdAddress = BUFFER;
        
        // Clear the BF flag
        BUFFER_FLAG = 0;
        
        //CKP = 1; We must have set this bit to one if SEN=1, but we work with SEN=0;
        
        while(D_nA == 1 && I2C_INTERRUPT);

        //while(STOP_BIT != 1){ We only recieve one command so we only receive one byte
            I2C_INTERRUPT = 0;
            
            uint8_t rcvdData = BUFFER;

            BUFFER_FLAG = 0;
        //}
        
        //With the rcvdDada (previous command send by the master in the previous i2c transmission)
        switch (rcvdData){
            case VOLTAGE_MEAS:
                //adcResponse = cuantifyValue(ADC_GetConversion(Voltage_Out));
                adcResponse = ADC_CuantifyValue(ADC_GetConversion(Voltage_Out)/100);
                break;

            case CURRENT_MEAS:
                //A conversion to voltage is needed
                //V_received=I*R   R=0.01ohms  G=100 V*G=V_received
                adcResponse = ADC_CuantifyValue((ADC_GetConversion(Current_Out)));
                break;

            case POL_STATE:
                //If RA5 pin output is '1' (Enabled)  ->  return '0x01'
                //If RA5 pin output is '0' (Disabled) ->  return '0x00'
                dataResponse = PORTAbits.RA5;
                break;

            case DISABLE:
                if(LATAbits.LATA5 == '0')
                    dataResponse = 0x0;
                else
                {
                    //Set RA5 output to '0'
                    LATAbits.LATA5 = 0;
                    dataResponse = 0x1;
                }
                break;

            case ENABLE:
                if(LATAbits.LATA5 == '1')
                    dataResponse = 0x0;
                else
                {
                    //Set RA5 output to '0'
                    LATAbits.LATA5 = 1;
                    dataResponse = 0x1;
                }
                break;
        } 
    }
    // Wait until the master read state is set
    while(I2C_INTERRUPT != 1 && R_nW != 1);

    //We have received an interrupt and it matches the device address
    //Master waits for an answer for the previous command send.

    //Interrupt bit cleared
    I2C_INTERRUPT = 0;

    //SSP1CON2bits.ACKDT = 1;
    // Read the rcvdAddress
    rcvdAddress = BUFFER;
    // Clear the BF flag
    BUFFER_FLAG = 0;
    //CKP = 1; We must have set this bit to one if SEN=1, but we work with SEN=0;  
    
    if(rcvdData == (0x0A || 0x0B)){
       
        BUFFER = adcResponse & 0x00FF;
        CKP = 1;
        // Wait until nACK is received
        while(ACKSTAT != 0);
        
        BUFFER = (adcResponse >> 8) && 0x00FF;
        CKP = 1;   
        //writeI2C(adcResponse & 0x00FF);
        //writeI2C(adcResponse >> 8);
    }          
    else{
        BUFFER = dataResponse;
        CKP = 1;
        //writeI2C(dataResponse);    
    }
    
    I2C_INTERRUPT = 0;
    
}