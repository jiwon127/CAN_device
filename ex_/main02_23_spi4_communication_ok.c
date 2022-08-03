#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>

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

    volatile short garbage = 0;
    volatile short realData = 0;

    while(1)

    {


        spi_cs_enable();

        uart_tx_str("first\r\n");
        uart_tx_hex( spi_read_16(0xc0000));
        uart_tx_hex( spi_read_32(0xc0000));

/*
        spi_send_byte(0x0c);
        garbage = spi_receive_byte();
        uart_tx_str("Garbage 1: ");uart_tx_hex(garbage);uart_tx_str("\r\n");

        spi_send_byte(0x00);
        garbage = spi_receive_byte();
        uart_tx_str("Garbage 2: ");uart_tx_hex(garbage);uart_tx_str("\r\n");

        spi_send_byte(0x00);//0x01
        garbage = spi_receive_byte();
        uart_tx_str("Garbage 3: ");uart_tx_hex(garbage);uart_tx_str("\r\n");

        spi_send_byte(0x0);
        garbage = spi_receive_byte();
        uart_tx_str("Garbage 4: ");uart_tx_hex(garbage);uart_tx_str("\r\n");


        spi_send_byte(0x0);
        realData = spi_receive_byte();
        uart_tx_str("Real Data1: ");uart_tx_hex(realData);uart_tx_str("\r\n");

        spi_send_byte(0x0);
        realData = spi_receive_byte();
        uart_tx_str("Real Data2: ");uart_tx_hex(realData);uart_tx_str("\r\n");

        spi_send_byte(0x0);
        realData = spi_receive_byte();
        uart_tx_str("Real Data3: ");uart_tx_hex(realData);uart_tx_str("\r\n");

        spi_send_byte(0x0);
        realData = spi_receive_byte();
        uart_tx_str("Real Data4: ");uart_tx_hex(realData);uart_tx_str("\r\n");

        while(SSI0_SR_R&SSI_SR_BSY){}
*/

        spi_cs_disable();



    }

    while(1){}
}





