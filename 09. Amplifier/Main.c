///////////////////////////////////////////////////////////////////////////
//  Product/Project : Amplifier                                          //
///////////////////////////////////////////////////////////////////////////
//
//  File Name    : Main.c
//  Version      : 1.0
//  Description  : Use the potmeters to set the amplification and show 
//				   this on VU meter leds
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
#include    "Delay.h"
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

struct
{
    // DMA buffer for A/D converter results
    int ADC1_0; // S/H channel 0: potmeter right
    int ADC1_1; // S/H channel 1: audio left
    int ADC1_2; // S/H channel 2: audio right
    int ADC1_3; // S/H channel 3: potmeter left
} DMAbuffer __attribute__((space(dma)));

#define	FIRST	500
#define	DIFF	500

void VU_meter_amp(unsigned short l,unsigned short r)
{
    unsigned short lvu, rvu;

    // Convert number to led value
    lvu = 0;
    if (l > FIRST + 0 * DIFF) lvu = VU_A1;
    if (l > FIRST + 1 * DIFF) lvu = VU_A2;
    if (l > FIRST + 3 * DIFF) lvu = VU_A3;
    if (l > FIRST + 6 * DIFF) lvu = VU_A4;
    if (l > FIRST + 10 * DIFF) lvu = VU_A5;
    if (l > FIRST + 15 * DIFF) lvu = VU_A6;
    if (l > FIRST + 21 * DIFF) lvu = VU_A7;
    if (l > FIRST + 28 * DIFF) lvu = VU_A8;
    rvu = 0;
    if (r > FIRST + 0 * DIFF) rvu = VU_B1;
    if (r > FIRST + 1 * DIFF) rvu = VU_B2;
    if (r > FIRST + 3 * DIFF) rvu = VU_B3;
    if (r > FIRST + 6 * DIFF) rvu = VU_B4;
    if (r > FIRST + 10 * DIFF) rvu = VU_B5;
    if (r > FIRST + 15 * DIFF) rvu = VU_B6;
    if (r > FIRST + 21 * DIFF) rvu = VU_B7;
    if (r > FIRST + 28 * DIFF) rvu = VU_B8;

    Shiftleds(lvu | rvu);
}

void Init_ADC_amp(void)
{
    AD1PCFGL = 0xFFE0; // AN0-AN4 analog inputs
    AD1CON1 = 0x11EC; // A/D module disabled
    /* 	10-bit conversion
            Signed integer format
            Auto-convert mode
            Simultaneous sampling
            Sampling starts automatic
     */
    AD1CSSL = 0x0000; // No input scanning
    AD1CON3 = 0x0203; // Systemclock for ADC
    /* 	Sampletime 3*Tad
            Tad 4*Tcy (100 ns)
            Sampletime = 4*12*Tad+Tsamp = 51 Tad = 5.1 us (200ks/s)
     */
    AD1CON2 = 0x0200; // AVdd and AVss as ADC reference voltages
    AD1CON4 = 0x0000; // 1 word buffer for each analog input
    AD1CHS123 = 0x0000; // Vref- as negative input
    /* 	AN0 = S/H channel 1 positive input (audio left)
            AN1 = S/H channel 2 positive input (audio right)
            AN2 = S/H channel 3 positive input (potmeter left)
     */
    AD1CHS0 = 0x0003; // Vref- as negative input
    // AN3 = S/H channel 0 positive input (potmeter right)
}

void Init_DAC_amp(void)
{
    DMA0CON = 0x0020;
    /* 	DMA channel disabled
        Mode: peripheral indirect, continuous, no ping-pong
     */
    DMA0PAD = (int) &ADC1BUF0; // DMA address = ADC1BUF0
    DMA0CNT = 3; // 4 DMA requests: 4 buffers, each 1 word
    DMA0REQ = 13; // ADC1 = DMA request source
    DMA0STA = __builtin_dmaoffset(&DMAbuffer);
    // Start DMA and A/D modules
    DMA0CONbits.CHEN = 1; // Enable DMA
    AD1CON1bits.ADON = 1; // Start A/D module
    // Initialize D/A convertor
    ACLKCONbits.SELACLK = 0; // Fast RC with PLL as aux. osc. divider source (80 MHz)
    ACLKCONbits.ASRCSEL = 1; // Primair oscillator as aux. osc. source
    ACLKCONbits.APSTSCLR = 0b111; // Aux. oscillator divider 1
    DAC1CONbits.DACFDIV = 3; // DAC clock divider 4
    DAC1DFLT = 0x0000; // Default value
    DAC1CONbits.FORM = 1; // Signed integer
    DAC1STATbits.LOEN = 1; // Left output enabled
    DAC1STATbits.ROEN = 1; // Right output enabled
    DAC1CONbits.DACEN = 1; // Start D/A module
}

int main(void)
{
    int i;
    int l, r, lmax = 0, rmax = 0;
    long l_offset = 0, r_offset = 0;

    Init_port();
    Init();
    Init_timers();
    Init_ADC_amp();
    Init_DAC_amp();

    while (1)
    {
        // Wait untill conversion = complete
        while (!IFS0bits.DMA0IF)
        {
            DAC(l, r);
        }
        // Erase DMA flag
        IFS0bits.DMA0IF = 0;

        // New DAC values
        // +512 because ADC format = signed integer
        l = (DMAbuffer.ADC1_1 - r_offset) * ((DMAbuffer.ADC1_3 + 512) >> 3);
        // +512 because ADC format = signed integer
        r = (DMAbuffer.ADC1_2 - l_offset) * ((DMAbuffer.ADC1_0 + 512) >> 3);
        DAC(l, r);

        // Peak detection
        if (l > lmax) lmax = l;
        if (r > rmax) rmax = r;

        if (i++ > 10000)
        {
            VU_meter_amp(lmax, rmax);
            setRGB(lmax >> 7, 0, rmax >> 7);
            i = 0;
            lmax = 0;
            rmax = 0;
        }
    }
    return (0);
}
