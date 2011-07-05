///////////////////////////////////////////////////////////////////////////
//  Product/Project : Sine                                               //
///////////////////////////////////////////////////////////////////////////
//
//  File Name    : Main.c
//  Version      : 1.0
//  Description  : Generate a sine wave through D/A converters,
//				   potmeter 1 defines the frequency
//  Author       : Jim Clermonts
//  Target       : dsPIC33FJ64GP802
//  Compiler     : MPLAB C Compiler for PIC24 and dsPIC v3.25 in LITE mode
//  IDE          : MPLAB X IDE Beta4.0
//  Programmer   : uBRD bootloader
//  Last Updated : 09-01-2011
//
//  Copyright (c) Staronic 2011.
//  All rights are reserved. Reproduction in whole or in part is
//  prohibited without the written consent of the copyright owner.
///////////////////////////////////////////////////////////////////////////
#include    "math.h"
#include    "Peripherals.h"
#include    "uBRD.h"

//Configuration-for-programmer--------------------------
_FBS(RBS_NO_RAM & BSS_NO_FLASH & BWRP_WRPROTECT_OFF);               // No boot ram, flash, write protection
_FSS(RSS_NO_RAM & SWRP_WRPROTECT_OFF);                              // No protected sections
_FGS(GSS_OFF & GCP_OFF & GWRP_OFF);                                 // No code protection
_FOSCSEL(FNOSC_FRCPLL & IESO_OFF);                                  // FRC Oscillator with PLL, no two-speed oscillator
_FOSC(FCKSM_CSDCMD & IOL1WAY_OFF & OSCIOFNC_ON  & POSCMD_NONE);     // Clock Switching and Fail Safe Clock Monitor is disabled
                                                                    // Multiple I/O reconfigurations admitted                                                                  // OSC2 Pin is digital I/O														                      // Primaire Oscillator Mode: none
_FWDT(FWDTEN_OFF & WINDIS_OFF);                                     // Watchdog Timer off
_FPOR(ALTI2C_ON & FPWRT_PWR64);                                     // 64 ms power-up timer
_FICD(JTAGEN_OFF & ICS_PGD2);                                       // JTAG program port off
//------------------------------------------------------

#define PI		3.141592653589793
#define BUFSIZE		8000
#define	MAX_VAL		10000
int buf[BUFSIZE] __attribute__((far));

int main(void)
{
    int value, step, idx;

    Init_port();
    Init();
    Init_timers();
    Init_ADC_sine();
    Init_DAC_sine();

    for (idx = 0; idx < BUFSIZE; idx++)
    {
        buf[idx] = (int) (double) (sin(2 * PI * idx / (BUFSIZE))*(double) MAX_VAL);
    }
    idx = 0;

    while (1)
    {
        // Wait for timeout of Timer4
        while (!IFS1bits.T4IF)
        {
            DAC(value, value);
        }
        IFS1bits.T4IF = 0;

        // Define stepwidth
        step = 20 + (ADC1BUF0 + 512);
        // Define index in buffer
        idx = idx + step;
        if (idx >= BUFSIZE) idx -= BUFSIZE;
        value = buf[idx];
        // New value to ADC converters
        DAC(value, value);
    }
    return (0);
}
