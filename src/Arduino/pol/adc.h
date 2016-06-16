/* 
 * File:   adc.h
 * Author: Albert Sanchez
 * Micro:  PIC12F1822
 */


#ifndef _ADC_H
#define	_ADC_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define conversionConstant 0.004887585533 // (5/1023)

typedef enum
{
    Voltage_Out =  0x0,
    Current_Out =  0x3
} adc_channel;


/**
  @Description
    This routine initializes the ADC.
 
  @Param
    None

  @Returns
    None
*/
void ADC_Init(void);

/**
  @Description
    This routine is used to select desired channel for conversion and
    to start the A/D conversion 
  
  @Param
    The channel where the conversion has to be made. The names
    of the channel are defined in the adc.h


  @Returns
    None
*/
void ADC_StartConversion(adc_channel channel);


/**
  @Description
    This routine is used to select desired channel for the conversion
    and to get the analog to digital converted value.
    
  @Param
    The channel where the conversion has been made. The names
    of the channel are defined in the adc.h

  @Returns
    Returns the converted value (from 0 to 1023).
*/
uint16_t ADC_GetConversion(adc_channel channel);

/**
  @Description
    This routine is used to convert the ADC value (from 0 to 1023)
    to a transmittable value. 
 
  @Param
    The binary result of the conversion

  @Returns
    Returns a 16 bits value with the integer value in the higher 8 bits and the
    decimal value in the lower 8 bits.
*/
uint16_t ADC_CuantifyValue(uint16_t adcReturned);

#endif	//_ADC_H

/**
 End of File
*/


