/*
 * File:   main.c
 * Author: AlbertSanchez
 * Micro:  PIC12F1822
 */


#include <xc.h>
#include "pic12f1822.h"
#include "init.h"

void main(void) 
{
    
    System_Init();
    while(true)
    {
        SLEEP();
    }
}
