///////////////////////////////////////////////////////////////////////////
//  Product/Project : RS232                                              //
///////////////////////////////////////////////////////////////////////////
//
//  File Name    : Main.c
//  Version      : 1.0
//  Description  : Echoes characters from the pc
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
#include    "Peripherals.h"
#include    "string.h"
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

/*  Windows: start hyperterminal or PuTTY (found in this folder)
 *
 *  Connection type: Serial
 *  Bits per second: 115200
 *  Data bits:       8
 *  Parity:          None
 *  Stop bits:       1
 *  Flow control:    Hardware
 */
int main(void)
{
    char c;
	
    Init_port();
    Init();
    Init_UART();
	
    Putstr("\n\r");
    Putstr("**********************************\n\r");
    Putstr("*       uBOARD main menu         *\n\r");
    Putstr("* Press g to turn on VU leds     *\n\r");
    Putstr("**********************************\n\r");
    
    while (1)
    {
        // Get character from hyperterminal
        c=Getcharacter();
		// Send character back to hyperterminal
        Putcharacter(c);
        if (c=='\n') Putcharacter('\r');
		if (c=='\r') Putcharacter('\n');

		// Turn on all VU leds
		if (c=='g')
		Shiftleds(0xFFFF);
    }
    return(0);
}
