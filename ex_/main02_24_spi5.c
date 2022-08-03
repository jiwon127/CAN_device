#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>
#include <header/eve2_lib.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>



int main()
{
    Sysctl_Clock();
    spi_init_conf();
    uart_init();
    uart_tx_str("Hello can start!!\r\n");

    SPI_LCD_ACTIVE();
    SPI_LCD_CLKEXT();


    uart_tx_str("first\r\n");
    spi_read_8(0xc0002);
    uart_tx_hex( spi_read_16(0xc0000));
    uart_tx_hex( spi_read_32(0xc0000));



    spi_write_16(REG_HCYCLE,548);
    spi_write_16(REG_HCYCLE, 548);
    spi_write_16(REG_HOFFSET, 43);
    spi_write_16(REG_HSYNC0, 0);
    spi_write_16(REG_HSYNC1, 41);
    spi_write_16(REG_VCYCLE, 292);
    spi_write_16(REG_VOFFSET, 12);
    spi_write_16(REG_VSYNC0, 0);
    spi_write_16(REG_VSYNC1, 10);
    spi_write_8(REG_SWIZZLE, 0);
    spi_write_8(REG_PCLK_POL, 1);
    spi_write_8(REG_CSPREAD, 1);
    spi_write_16(REG_HSIZE, 480);
    spi_write_16(REG_VSIZE, 272);
    spi_write_32(RAM_DL+0,CLEAR_COLOR_RGB(0,0,0));
    spi_write_32(RAM_DL+4,CLEAR(1,1,1));
    spi_write_32(RAM_DL+8,DISPLAY());
    spi_write_8(REG_DLSWAP,DLSWAP_FRAME);//display list swap
    spi_write_8(REG_GPIO_DIR,0x80 | spi_read_8(REG_GPIO_DIR));
    spi_write_8(REG_GPIO,0x080 | spi_read_8(REG_GPIO));//enable display bit
    spi_write_8(REG_PCLK,5);//after this display is visible on the LCD

    spi_write_8(REG_VOL_SOUND,0xFF); //set the volume to maximum
    spi_write_16(REG_SOUND, (0x6C<< 8) | 0x41); // C8 MIDI note on xylophone
    spi_write_8(REG_PLAY, 1);

    spi_write_32(RAM_DL + 0, CLEAR(1, 1, 1)); // clear screen
    spi_write_32(RAM_DL + 4, BEGIN(BITMAPS)); // start drawing bitmaps
    spi_write_32(RAM_DL + 8, VERTEX2II(220, 110, 31, 'F')); // ascii F in font 31
    spi_write_32(RAM_DL + 12, VERTEX2II(244, 110, 31, 'T')); // ascii T
    spi_write_32(RAM_DL + 16, VERTEX2II(270, 110, 31, 'D')); // ascii D
    spi_write_32(RAM_DL + 20, VERTEX2II(299, 110, 31, 'I')); // ascii I
    spi_write_32(RAM_DL + 24, END());
    spi_write_32(RAM_DL + 28, COLOR_RGB(160, 22, 22)); // change colour to red
    spi_write_32(RAM_DL + 32, POINT_SIZE(320)); // set point size to 20 pixels in radius
    spi_write_32(RAM_DL + 36, BEGIN(POINTS)); // start drawing points
    spi_write_32(RAM_DL + 40, VERTEX2II(192, 133, 0, 0)); // red point
    spi_write_32(RAM_DL + 44, END());
    spi_write_32(RAM_DL + 48, DISPLAY()); // display the image

    while(1){}
}





