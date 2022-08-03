#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Uart7.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/timer.h>


#include <header/FT81X_LCD.h>
#include <header/eve2_lib.h>
#include <header/my_eve2_lib.h>
#include <header/MatrixEve2Conf.h>

#include <header/asm.h>
#include <header/Hibernation.h>
#include <header/Flash_memory.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

unsigned char timer_irq_led_flag=0; //When using this comment out flag in timer.c

void timer0_handler(void)   //ISR
{
    //uart_7_tx(uart_7_rx());
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;

    //GPIO_PORTD_DEN_R = 0xff;

    GPIO_PORTD_DEN_R = 0xff-timer_irq_led_flag;
    timer_irq_led_flag++;

    uart_7_tx_hex((int)(0xff-timer_irq_led_flag));
    uart_7_tx_str("\r\n");
    if(timer_irq_led_flag >=0xff) timer_irq_led_flag=0;



}

void timer2_handler(void)   //ISR
{
    //uart_7_tx(uart_7_rx());

    TIMER2_ICR_R = TIMER_ICR_TATOCINT;

    GPIO_PORTD_DEN_R = timer_irq_led_flag;
    timer_irq_led_flag++;
    uart_7_tx_hex((int)timer_irq_led_flag);
    uart_7_tx_str("\r\n");
    if(timer_irq_led_flag >=0xff) timer_irq_led_flag=0;
}


int main()
{
    Sysctl_Clock();
    uart_7_init();

    Led(0);
    uart_7_tx_str("start\r\n");
    Timer0_init(0x007a1200);

    while(1)
    {
        //if(TIMER0_RIS_R & (0x00000001) ==1){}
        //uart_7_tx(uart_7_rx()); //MUST ADD THIS LINE FOR HIB
    }



    asm_rst();

    while(1){}
}


/*
ISR TIMER()
{
    //0.1SEC -> READ SPEED(CAN) -> * TIME = DISTANCE -> += RETURN
    // return value display on lcd
}

ISR DRIVING_FINISH()
{
    //CHECK GEAR, RPM(5 MIN), SPEED(5 MIN)
    //Turn off all init and conf
    //write distance data to flash
    //go to hib
}


ISR HIB()
{
    //maybe it does not use
}

ISR LCD_TOUCH()
{
    //if touch value is 1, come here
    //display a new screen
    //ask again
    //delete flash data
}


int main()
{
    Sysctl_Clock();
    uart_7_init();
    spi_init_conf();

    SPI_LCD_ACTIVE();
    FT81x_Init();
    ClearScreen();

    uint32_t flash_read[8];
    uint32_t  i;
    char can_data;
    char *can_data_ptr =can_data; //must little endian


    Flash_read(flash_read,2);

    while(1)
    {
        CanToSpiGear(&can_data_ptr);
        uart_7_tx_str(can_data_ptr);
        uart_7_tx('\n');
        make_character(1,can_data_ptr);
        delay_ms(500);
    }


    //asm_rst();

    while(1){}
}

*/

