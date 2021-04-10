/**
 * main.c
 *
 * ECE 3849 Lab 0
 * Rosana Pochat
 * 03/31/2021
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "Crystalfontz128x128_ST7735.h"
#include "buttons.h"
#include <stdio.h>

uint32_t gSystemClock; // [Hz] system clock frequency
volatile uint32_t gTime = 8345; // time in hundredths of a second

int main(void)
{
    IntMasterDisable();

    // Enable the Floating Point Unit, and permit ISRs to use it
    FPUEnable();
    FPULazyStackingEnable();

    // Initialize the system clock to 120 MHz
    gSystemClock = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, 120000000);

    Crystalfontz128x128_Init(); // Initialize the LCD display driver
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP); // set screen orientation

    tContext sContext;
    GrContextInit(&sContext, &g_sCrystalfontz128x128); // Initialize the grlib graphics context
    GrContextFontSet(&sContext, &g_sFontFixed6x8); // select font

    uint32_t time;  // local copy of gTime
    char str[50];   // string buffer
    char str1[50];
    int buttonsBinary;

    // full-screen rectangle
    tRectangle rectFullScreen = {0, 0, GrContextDpyWidthGet(&sContext)-1, GrContextDpyHeightGet(&sContext)-1};

    ButtonInit();
    IntMasterEnable();

    while (true) {
        GrContextForegroundSet(&sContext, ClrBlack);
        GrRectFill(&sContext, &rectFullScreen); // fill screen with black

        time = gTime; // read shared global only once
        int mm = time/6000;
        int ss = (time%6000)/100;
        int ff = ((time%6000)%100);

        buttonsBinary = decToBinary(gButtons);

        snprintf(str, sizeof(str), "Time = %02u:%02u:%02u", mm, ss, ff); // convert time to string
        GrContextForegroundSet(&sContext, ClrWhite); // white text
        GrStringDraw(&sContext, str, /*length*/ -1, /*x*/ 0, /*y*/ 0, /*opaque*/ false);

        snprintf(str1, sizeof(str1), "Buttons: %09u", buttonsBinary); // convert time to string
        GrStringDraw(&sContext, str1, /*length*/ -1, /*x*/ 0, /*y*/ 10, /*opaque*/ false);
        GrFlush(&sContext); // flush the frame buffer to the LCD
    }
}
