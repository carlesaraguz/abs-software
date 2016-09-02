/*
 * File:   adc.c
 * Author: AlbertSanchez
 * Micro:  PIC12F1822
 */

#include <xc.h>
#include "adc.h"



void ADC_Init(void)
{
    //Port RA0 configurated as an input (tri-stated))
    TRISAbits.TRISA0 = 1;
    //Port RA4 configurated as an input (tri-stated))
    TRISAbits.TRISA4 = 1;
    
    //Port RA0 configured as an analog input
    ANSELAbits.ANSA0 = 1;
    //Port RA4 configured as an analog input
    ANSELAbits.ANSA4 = 1;
    
    // GO_nDONE stop; ADON enabled ('-xxxxxx1'); CHS AN0 ('-00000xx');

    ADCON0 = 0x01;
    
    // ADFM right ('1xxx--xx'); ADCS FOSC/16 ('x101--xx'); ADPREF VDD ('xxxx--00');
    ADCON1 = 0xD0;
    
    // ADRESH 0; (Force '0' to the ADC RESULT REGISTER HIGH) 
    ADRESH = 0x00;
    
    // ADRESL 0; (Force '0' to the ADC RESULT REGISTER LOW)
    ADRESL = 0x00;
    
}

void ADC_StartConversion(adc_channel channel)
{
    // select the A/D channel
    ADCON0bits.CHS = channel;    
    // Turn on the ADC module
    ADCON0bits.ADON = 1;

    // Start the conversion
    ADCON0bits.GO_nDONE = 1;
    
}

uint16_t ADC_GetConversion(adc_channel channel)
{
    ADC_StartConversion(channel);

    // Wait for the conversion to finish
    while(ADCON0bits.GO_nDONE);
    
    // Conversion finished, return the result
    return((ADRESH << 8) + ADRESL);
}

uint16_t ADC_CuantifyValue(uint16_t adcReturned)
{
    float convertedValue;
    uint8_t integerValue;
    uint8_t decimalValue;
  
    convertedValue = conversionConstant * adcReturned;
    integerValue = convertedValue;
    decimalValue = (convertedValue - integerValue) * 100; 
    
    return ((0x00FF & decimalValue) + (integerValue << 8));
    
}
/*
 End of File
*/

