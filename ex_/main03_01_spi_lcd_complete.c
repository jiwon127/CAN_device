#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>

#include <header/eve2_lib.h>
#include <header/MatrixEve2Conf.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


void ex(void)
{
    Send_CMD(CMD_DLSTART); // start a new display list
    Send_CMD(CLEAR_COLOR_RGB(140, 140, 140)); // set clear color
    Send_CMD(CLEAR(1, 1, 1)); // clear screen
    Send_CMD(COLOR_RGB(0, 0, 0));      //Change color to white for text
    Cmd_Gauge(80,60,50,0,5,4,30,100);
    Send_CMD(DISPLAY()); // display
    Send_CMD(CMD_SWAP);                      //Swap commands into RAM
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO

}
void make_character(uint8_t DotSize,const char* str)
{
    Send_CMD(CMD_DLSTART);                   //Start a new display list
    Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));      //Determine the clear screen color
    Send_CMD(CLEAR(1, 1, 1));                //Clear the screen and the curren display list
    Send_CMD(COLOR_RGB(0, 190, 0));        // change colour to blue
    Send_CMD(POINT_SIZE(DotSize * 16));      // set point size to DotSize pixels. Points = (pixels x 16)
    Send_CMD(BEGIN(POINTS));                 // start drawing point
    Send_CMD(TAG(1));                        // Tag the blue dot with a touch ID
    Send_CMD(VERTEX2F(DWIDTH / 4, DHEIGHT / 2));     // place blue point
    Send_CMD(END());                         // end drawing point
    Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text
    Cmd_Text(DWIDTH / 2, DHEIGHT / 2, 30, OPT_CENTER, str); //Write text in the center of the screen
    Send_CMD(DISPLAY());                     //End the display list
    Send_CMD(CMD_SWAP);                      //Swap commands into RAM
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
}

int main()
{
    Sysctl_Clock();
    spi_init_conf();
    uart_init();

    uart_tx_str("Hello can start!!\r\n");
    uart_tx_str("first\r\n");

    SPI_LCD_ACTIVE();
    FT81x_Init();

    ClearScreen();

    int i=0;
    int length=32;
    char a[32]={0,};
    char *b = a;

    while(1)
    {

        for(i=0;i<length;i++)
        {
            a[i] = uart_rx();
            if(a[i] == '\n') break;
        }

        uart_tx_str(a);


        make_character(30,b);
        if( *b == 'p') break;

    }

    make_character(1,"communication finish"); //20 character

    while(1){}
}


