#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>
#include <header/my_eve2_lib.h>
//#include <header/Flash_memory.h>

#include <header/eve2_lib.h>
#include <header/MatrixEve2Conf.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


#define FLASH_BASE_ADDR         ((volatile uint32_t*)0x00020000)
/*
static uint16_t flashKey_ = 0;

void FLASH_init(void)
{
    if (FLASH_BOOTCFG_R & 0x10) {
        flashKey_ = 0xa442;
    }
    else {
        flashKey_ = 0x71d5;
    }

}

void FLASH_Erase(uint32_t blockCount)
{
    if (flashKey_ == 0) {
        return;
    }
    uint32_t i;
    for ( i = 0; i < blockCount; i++) {

        FLASH_FMA_R &= 0xFFFC0000;

        FLASH_FMA_R |= ((uint32_t)FLASH_BASE_ADDR) + (i*1024);

        FLASH_FMC_R = (flashKey_ << 16) | 0x02;

        while (FLASH_FMC_R & 0x2);

    }
}


void FLASH_Write(const uint32_t* data, uint32_t wordCount)
{
    if (flashKey_ == 0) {
        return;
    }

    uint32_t blockCount = ((wordCount * sizeof(uint32_t)) / 1024) + 1;
    FLASH_Erase(blockCount);
    uint32_t i;
    for ( i = 0; i < wordCount; i++) {
        FLASH_FMD_R = data[i];

        FLASH_FMA_R &= 0xFFFC0000;
        FLASH_FMA_R |= (uint32_t)FLASH_BASE_ADDR + (i * sizeof(uint32_t));

        FLASH_FMC_R = (flashKey_ << 16) | 0x01;

        // Poll the WRITE bit until it is cleared.
        while (FLASH_FMC_R & 0x1);

    }
}



void FLASH_Read(uint32_t* data, uint32_t wordCount)
{
    uint32_t i;
    for (i = 0; i < wordCount; i++) {
        data[i] = FLASH_BASE_ADDR[i];
    }

}
*/

int main()
{
    Sysctl_Clock();

    int data_a[4]={0x12,0x34,0xab,0xcd};
    int data_b[4]={0,};
    int *data_c;

/*
    FLASH_init();
    FLASH_Write(data_a,5);
    FLASH_Read(data_b,5);
*/

    Flash_write(data_a,4);
    Flash_read(data_b,4);

    while(1){}
}


