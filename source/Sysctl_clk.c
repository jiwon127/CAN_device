#include <tm4c123gh6pm.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void Sysctl_Clock()
{
    SYSCTL_RCC_R =      0<<27 | //AGC = 0       //Sleep mode 에서만 사용. don't care
                        0<<11 | //BYPASS = 0;   //System Clock is from PLL, PLL: 400Mhz/2 = 200Mhz
                        9<<23 | //SYSDIV = 9    //This should be 20Mhz->System Clock
                        //4<<23 | //SYSDIV = 9    //This should be 20Mhz->System Clock
                        1<<22 | //USE USE SYSDIV = 1 //This MUST be enabled since PLL is used.


                        0<<20 | //USE PWMDIV = 0


                        0<<13 | //USE PWRDN = 0;//PLL is Operation Normal -> TURN ON.
                        //0x15<<6  | //Crystal = 0110 = 6 = 4Mhz Xtal. //10101 = 0x15 = 16Mhz USB.
                        6<<6|

                        1<<4  | //OSCSRC =0 ;   //PI OSC.
                        1<<0  | //MOSCDIS = 1;  //Main Oscillator is Disabled.
                        0;
    //NOW systemClock = 20Mhz!
}

void Sysctl_Clock_10MHz()
{
    SYSCTL_RCC_R =      0<<27 | //AGC = 0
                        9<<23 | //SYSDIV = 9    //This should be 20Mhz->System Clock
                        1<<22 | //USE SYSDIV = 1 //This MUST be enabled since PLL is used.
                        0<<20 | //USE PWMDIV = 0
                        0<<13 | //USE PWRDN = 0;//PLL is Operation Normal.
                        0<<11 | //BYPASS = 0;   //System Clock is from PLL.
                        6<<6  | //Crystal = 0110 = 6 = 4Mhz Xtal. //10101 = 0x15 = 16Mhz USB.
                        1<<4  | //OSCSRC =0 ;   //PI OSC.
                        1<<0  | //MOSCDIS = 1;  //Main Oscillator is Disabled.
                        0;

    SYSCTL_RCC2_R =     0<<31 | //AGC = 0
                        9<<30 | //SYSDIV = 9    //This should be 20Mhz->System Clock
                        1<<23 | //USE SYSDIV = 1 //This MUST be enabled since PLL is used.
                        0<<22 | //USE PWMDIV = 0
                        0<<14 | //The USB PLL operates normally
                        0<<13 | //The PLL operates normally.
                        6<<11 | //PLL Bypass 2
                        1<<4 | //Oscillator Source 2
                        0;

    //NOW systemClock = 10Mhz!
}
