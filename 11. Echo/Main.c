///////////////////////////////////////////////////////////////////////////
//  Product/Project : Echo                                               //
///////////////////////////////////////////////////////////////////////////
//
//  File Name    : Main.c
//  Version      : 1.0
//  Description  : Press the down switch to toggle echo on/off
//  Author       : Jim Clermonts
//  Target       : dsPIC33FJ64GP802
//  Compiler     : MPLAB C Compiler for PIC24 and dsPIC v3.25 in LITE mode
//  IDE          : MPLAB X IDE Beta4.0
//  Programmer   : uBRD bootloader
//  Last Updated : 16-01-2011
//
//  Copyright (c) Staronic 2011.
//  All rights are reserved. Reproduction in whole or in part is
//  prohibited without the written consent of the copyright owner.
///////////////////////////////////////////////////////////////////////////
#include    "Delay.h"
#include    "Peripherals.h"
#include    "uBRD.h"

//Configuration-for-programmer--------------------------
_FBS(RBS_NO_RAM & BSS_NO_FLASH & BWRP_WRPROTECT_OFF);               // No boot ram, flash, write protection
_FSS(RSS_NO_RAM & SWRP_WRPROTECT_OFF);                              // No protected sections
_FGS(GSS_OFF
& GCP_OFF & GWRP_OFF);                                              // No code protection
_FOSCSEL(FNOSC_FRCPLL & IESO_OFF);                                  // FRC Oscillator with PLL, no two-speed oscillator
_FOSC(FCKSM_CSDCMD & IOL1WAY_OFF & OSCIOFNC_ON  & POSCMD_NONE);     // Clock Switching and Fail Safe Clock Monitor is disabled
                                                                    // Multiple I/O reconfigurations admitted                                                                  // OSC2 Pin is digital I/O														                      // Primaire Oscillator Mode: none
_FWDT(FWDTEN_OFF & WINDIS_OFF);                                     // Watchdog Timer off
_FPOR(ALTI2C_ON & FPWRT_PWR64);                                     // 64 ms power-up timer
_FICD(JTAGEN_OFF & ICS_PGD2);                                       // JTAG program port off
//------------------------------------------------------

#define ISR __attribute__((interrupt))
// 40.000 kHz sample rate: 8000/40.000=0,2 sec delay
#define BUFFER 	8000 	

unsigned char c;
unsigned char last_c;
unsigned char count = 0;
unsigned char echo_on = 0;
unsigned int i = 0;
char lbuf[BUFFER] __attribute__((far));
char rbuf[BUFFER] __attribute__((far));

void __attribute__((interrupt, auto_psv)) ISR _T2Interrupt(void)
{  
	AD1CHS0bits.CH0SA = 0b00000;    // AN0 = channel 0
    AD1CON1bits.SAMP = 1;           // Start sampling
    while (AD1CON1bits.DONE == 0);  // Wait untill conversion is complete

    if (echo_on)
    {
		// Create the echo
        DAC1RDAT = (ADC1BUF0 << 3)+(rbuf[i] << 7); 
    } 
	else
    {
		// Send input to output, no echo
        DAC1RDAT = ADC1BUF0 << 3; 
    }

	// Store samples
    rbuf[i] = ADC1BUF0 >> 4; 

	// AN1 = channel 0
    AD1CHS0bits.CH0SA = 0b00001;    
	// Start sampling
    AD1CON1bits.SAMP = 1;           
	// Wait untill conversion complete
    while (AD1CON1bits.DONE == 0);  

    if (echo_on)
    {
		// Create the echo
        DAC1LDAT = (ADC1BUF0 << 3)+(lbuf[i] << 7);
    } else
    {
		//// Send input to output, no echo
        DAC1LDAT = ADC1BUF0 << 3; 
    }
	// Store samples
    lbuf[i] = ADC1BUF0 >> 4;

    if (i >= (BUFFER - 1))
    {
        i = 0;
    }
    else
    {
        i++;
    }

    if (count >= 10)
    {
        count = 0;
    }
    else
    {
        count++;
    }
	// Timer2 Interrupt flag low
    IFS0bits.T2IF = 0; 
}

int main(void)
{
    // Initialize digital I/O
    T_LEDR;
    LEDR = 1;
    T_LEDB;
    LEDB = 0;

    Init_port();
    Init();
    Init_timer_echo();
    Init_DAC_echo();
    Init_ADC_echo();
    Init_mcp();

    while (1)
    {
        if (count == 0)
        {
            // Check if switch is pressed
            last_c = c;
            c = Read23X08_17(GPIOB,PORTEXP_UBRD);
        }
        // Delay for switch
        if (count == 10)
        {
            if (((c & 0b00000001) == 0) && (c != last_c))
            {
				// Toggle echo modus
                echo_on =! echo_on; 
				// Blue led is on when echo is activated
                LEDB = !LEDB;
				// Else red led on
                LEDR = !LEDR;
            }
        }
    }
}
