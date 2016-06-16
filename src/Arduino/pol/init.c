/*
 * File:   init.c
 * Author: AlbertSanchez
 * Micro:  PIC12F1822
 */

#include <xc.h>
#include "init.h"

void System_Init(void){
    Config_Bits();
    Oscillator_Init();
    ADC_Init();
    Interrupts_Init();
    I2C_Init();
    
}

void Config_Bits(void){
    // CONFIG1
#pragma config FOSC = 0b100  // Oscillator Selection->INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE = 0b00   // Watchdog Timer Enable->WDT disabled
#pragma config PWRTE = 1     // Power-up Timer Enable->PWRT disabled
#pragma config MCLRE = 1     //MCLR pin enabled// MCLR Pin Function Select->MCLR/VPP pin function is digital input
#pragma config CP = 1        // Flash Program Memory Code Protection->Program memory code protection is disabled
#pragma config CPD = 1       // Data Memory Code Protection->Data memory code protection is disabled
#pragma config BOREN = 0b00  // Brown-out Reset Disable->Brown-out Reset disabled
#pragma config CLKOUTEN = 1  // Clock Out Enable->CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin
#pragma config IESO = 1      // Internal/External Switchover->Internal/External Switchover mode is enabled
#pragma config FCMEN = 1     // Fail-Safe Clock Monitor Enable->Fail-Safe Clock Monitor is enabled

    // CONFIG2
#pragma config WRT = 0b11    // Flash Memory Self-Write Protection->Write protection off
#pragma config PLLEN = 0     // PLL Enable->4x PLL disabled
#pragma config STVREN = 1    // Stack Overflow/Underflow Reset Enable->Stack Overflow or Underflow will cause a Reset
#pragma config BORV = 1      // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (Vbor), low trip point selected.
#pragma config LVP = 0       // Low-Voltage Programming Enable->High-voltage on MCLR/VPP must be used for programming
}

void Oscillator_Init(void){
    // SCS INTOSC; SPLLEN disabled; IRCF 16MHz_HF; 
    OSCCON = 0x7A;
    // LFIOFR disabled; HFIOFL not stable; OSTS intosc; PLLR disabled; HFIOFS not stable; HFIOFR disabled; MFIOFR disabled; T1OSCR disabled; 
    OSCSTAT = 0x00;
    // TUN 0; 
    OSCTUNE = 0x00;
    // Set the secondary oscillator
}

