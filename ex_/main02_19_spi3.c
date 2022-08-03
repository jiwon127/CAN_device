#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
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

/*    while(1)
 */
    {
        GPIO_PORTA_DATA_R = (GPIO_PORTA_DATA_R&(~(unsigned long)0x08)) | 1<<3;  //nCS = 0;

        volatile short garbage = 0;
        GPIO_PORTA_DATA_R = (GPIO_PORTA_DATA_R&(~(unsigned long)0x08)) | 0<<3;  //nCS = 0;
        spi_send_byte(0x00);
        spi_send_byte(0x00);
        spi_send_byte(0x00);

        delay_ms(5000);

        spi_send_byte(0xc);
        garbage = spi_receive_byte();
        uart_tx_str("Garbage 1: ");uart_tx_hex(garbage);uart_tx_str("\r\n");

        spi_send_byte(0x0);
        garbage = spi_receive_byte();
        uart_tx_str("Garbage 2: ");uart_tx_hex(garbage);uart_tx_str("\r\n");

        spi_send_byte(0x0);
        garbage = spi_receive_byte();
        uart_tx_str("Garbage 3: ");uart_tx_hex(garbage);uart_tx_str("\r\n");

        spi_send_byte(0x0);
        garbage = spi_receive_byte();
        uart_tx_str("Garbage 4: ");uart_tx_hex(garbage);uart_tx_str("\r\n");

        spi_send_byte(0x0);
        volatile short realData = spi_receive_byte();
        uart_tx_str("Real Data: ");uart_tx_hex(realData);uart_tx_str("\r\n");


        while(SSI0_SR_R&SSI_SR_BSY){}
        GPIO_PORTA_DATA_R = (GPIO_PORTA_DATA_R&(~(unsigned long)0x08)) | 1<<3;  //nCS = 1; End of communication;

        //return realData;
    }

    while(1){}
}


