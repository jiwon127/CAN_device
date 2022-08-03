#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Uart7.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>
#include <header/eve2_lib.h>
#include <header/MatrixEve2Conf.h>
#include <header/asm.h>
#include <header/Hibernation.h>
#include <header/Flash_memory.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>



int main()
{
    Sysctl_Clock();
    uart_7_init();

    uart_7_tx_str("start\r\n");

    uint32_t a[8];
    uint32_t b[8]; //must little endian

    uint32_t  i;

    //uart_7_rx_str((char*)b,2);

    //Flash_write(b,2);
    Flash_read(a,2);

    delay_ms(500);
    uart_7_tx_str(a);
    uart_7_tx_str("\r\n");


    //asm_rst();

    while(1){}
}





