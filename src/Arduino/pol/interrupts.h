/* 
 * File:   interrupts.h
 * Author: AlbertSanchez
 * Micro:  PIC12F1822
 */

#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#include <xc.h>
#include "i2c.h"

/**
  @Description
    This routine initializes the interrupts in the peripherals.
    This routine should only be called once during system initialization.

  @Param
    None

  @Returns
    None
*/
void Interrupts_Init(void);

/**
  @Description
    This is the main interrupt routine when the master requests some action
    to the desired slave

  @Param
    None

  @Returns
    None
*/
void interrupt I2C_MSTR_REQ(void);

#endif	/* INTERRUPTS_H */

