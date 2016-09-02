/* 
 * File: init.h  
 * Author: Albert Sanchez
 * Micro:  PIC12F1822 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef INIT_H
#define	INIT_H

#include <xc.h>   
#include "adc.h"
#include "interrupts.h"
#include "i2c.h"


/**
  @Description
    This routine does all the system initialization.

  @Param
    None

  @Returns
    None
*/
void System_Init(void);

/**
  @Description
    This routine fix all the configurated bits in the desired state.
    This routine must be called inside and at the top of System_Init().
    This routine should only be called once.

  @Preconditions
    None

  @Param
    None

  @Returns
    None
*/
void Config_Bits(void);

/**
  @Description
    This routine fix all desired registers to the desired value.
    This routine must be called inside and at the next to Config_Bits().
    This routine should only be called once.

  @Preconditions
    None

  @Param
    None

  @Returns
    None
*/
void Oscillator_Init(void);


#endif	/* INIT_H */

