#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>
#include <header/eve2_lib.h>
#include <header/MatrixEve2Conf.h>
#include <header/eve2_lib.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

void calibrate_once(void)
{
    spi_write_32(REG_TOUCH_TRANSFORM_A +RAM_REG, 0x000053FB);
    spi_write_32(REG_TOUCH_TRANSFORM_B +RAM_REG, 0xFFFFFCE7);
    spi_write_32(REG_TOUCH_TRANSFORM_C +RAM_REG, 0x00033F47);
    spi_write_32(REG_TOUCH_TRANSFORM_D +RAM_REG, 0xFFFFFF1B);
    spi_write_32(REG_TOUCH_TRANSFORM_E +RAM_REG, 0xFFFFBA38);
    spi_write_32(REG_TOUCH_TRANSFORM_F +RAM_REG, 0x01073724);
}

void hongCalibrate(void)
{
    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR_COLOR_RGB(0,0,0));
    Send_CMD(CLEAR(1,1,1));
    Cmd_Text(DWIDTH / 2 , DHEIGHT / 2, 28, OPT_CENTER, "Tap on the dots");
    Cmd_Calibrate(0);
    // This widget generates a blocking screen that doesn't unblock until 3 dots have been touched
    Send_CMD(DISPLAY());
    Send_CMD(CMD_SWAP);
    UpdateFIFO();
    // Trigger the CoProcessor to start processing commands out of the FIFO

    Wait4CoProFIFOEmpty();
}

void hongButton(int x, int y)
{
    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR_COLOR_RGB(0,0,0));
    Send_CMD(CLEAR(1,1,1));
    //Send_CMD(COLOR_RGB(190, 0, 0));        // change char color to blue
    Send_CMD(TAG(1));                        // Tag the blue dot with a touch ID
    Cmd_Button(x, y, 80, 60, 28, 0,"Press!");
    Send_CMD(DISPLAY());
    Send_CMD(CMD_SWAP);
    UpdateFIFO();
    // Trigger the CoProcessor to start processing commands out of the FIFO

    Wait4CoProFIFOEmpty();

}

void make_gauge(void)
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

void make_character(unsigned char DotSize,const char* str)
{

    Send_CMD(CMD_DLSTART);                   //Start a new display list
    Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));      //Determine the clear screen color
    Send_CMD(CLEAR(1, 1, 1));                //Clear the screen and the curren display list

    Send_CMD(COLOR_RGB(190, 0, 0));        // change color to blue
    Send_CMD(POINT_SIZE(DotSize * 16));      // set point size to DotSize pixels. Points = (pixels x 16)
    Send_CMD(BEGIN(POINTS));                 // start drawing point

    Send_CMD(TAG(1));                        // Tag the blue dot with a touch ID
    //Cmd_Button(x, y, 80, 60, 28, 0,"Press!");

    Send_CMD(VERTEX2F(DWIDTH / 6, DHEIGHT / 2));     // place blue point
    Send_CMD(END());                         // end drawing point
    Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text
    Cmd_Text(DWIDTH / 2 , DHEIGHT / 2, 30, OPT_CENTER, str); //Write text in the center of the screen
    Send_CMD(DISPLAY());                     //End the display list
    Send_CMD(CMD_SWAP);                      //Swap commands into RAM
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
}

void display_accel_trans_rpm(unsigned char DotSize,const char* str,const char* str2, const char* str3)
{


    Send_CMD(CMD_DLSTART);                   //Start a new display list
    Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));      //Determine the clear screen color
    Send_CMD(CLEAR(1, 1, 1));                //Clear the screen and the curren display list

    Send_CMD(TAG(1));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH / 10 - 10, DHEIGHT *2/ 3, 80, 60, 28, 0,"Data");

    Send_CMD(TAG(3));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH *6 / 9 + 10, DHEIGHT *2 / 3  , 80, 60, 28, 0,"Reset");

    Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text

    Cmd_Text(DWIDTH / 4 , DHEIGHT / 4, 29, OPT_CENTER, str); //Write text in the center of the screen
    Cmd_Text(DWIDTH * 3 / 4  , DHEIGHT / 4  , 29, OPT_CENTER, str2); //Write text in the center of the screen
    Cmd_Text(DWIDTH / 2  , DHEIGHT / 2   , 31, OPT_CENTER, str3); //Write text in the center of the screen

    Send_CMD(DISPLAY());                     //End the display list
    Send_CMD(CMD_SWAP);                      //Swap commands into RAM
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
}

void display_fuel(unsigned char DotSize,const char* str,const char* str2, const char* str3)
{

    Send_CMD(CMD_DLSTART);                   //Start a new display list
    Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));      //Determine the clear screen color
    Send_CMD(CLEAR(1, 1, 1));                //Clear the screen and the curren display list

    Send_CMD(TAG(2));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH / 10 - 10, DHEIGHT *2/ 3, 80, 60, 28, 0,"Speed");

    Send_CMD(TAG(3));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH *6 / 9 + 10, DHEIGHT *2 / 3  , 80, 60, 28, 0,"Reset");

    Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text

    Cmd_Text(DWIDTH / 4 , DHEIGHT / 4 -24, 28, OPT_CENTER, "Engine oil"); //Write text in the center of the screen
    Cmd_Text(DWIDTH / 4 , DHEIGHT / 4, 28, OPT_CENTER, str); //Write text in the center of the screen
    Cmd_Text(DWIDTH / 4 , DHEIGHT / 4 +24, 28, OPT_CENTER, "/15,000km"); //Write text in the center of the screen

    Cmd_Text(DWIDTH * 3 / 4 , DHEIGHT / 4 -24, 28, OPT_CENTER, "Mission oil"); //Write text in the center of the screen
    Cmd_Text(DWIDTH * 3 / 4 , DHEIGHT / 4  , 28, OPT_CENTER, str2); //Write text in the center of the screen
    Cmd_Text(DWIDTH * 3 / 4 , DHEIGHT / 4 +24 , 28, OPT_CENTER, "/40,000km"); //Write text in the center of the screen

    Cmd_Text(DWIDTH / 2 , DHEIGHT *2 / 3 -24, 28, OPT_CENTER, "Break oil"); //Write text in the center of the screen
    Cmd_Text(DWIDTH / 2 , DHEIGHT *2 / 3  , 28, OPT_CENTER, str3); //Write text in the center of the screen
    Cmd_Text(DWIDTH / 2 , DHEIGHT *2 / 3 +24 , 28, OPT_CENTER, "/60,000km"); //Write text in the center of the screen

    Send_CMD(DISPLAY());                     //End the display list
    Send_CMD(CMD_SWAP);                      //Swap commands into RAM
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
    //Wait4CoProFIFOEmpty();
    //delay_ms(10);
}


void display_fuel_reset(unsigned char DotSize,const char* str,const char* str2, const char* str3)
{

    Send_CMD(CMD_DLSTART);                   //Start a new display list
    Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
    Send_CMD(CLEAR_COLOR_RGB(100, 100, 100));      //Determine the clear screen color
    Send_CMD(CLEAR(1, 1, 1));                //Clear the screen and the curren display list


    Send_CMD(TAG(4));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH / 4 - 50 , DHEIGHT  / 4 , 100, 60, 28, 0,"Engine");

    Send_CMD(TAG(5));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH *3 / 4 - 50 , DHEIGHT / 4, 100, 60, 28, 0,"Mission");

    Send_CMD(TAG(6));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH / 2 - 50 , DHEIGHT *2 / 3, 100, 60, 28, 0,"Break");

    Send_CMD(TAG(7));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH *6 / 7 - 50 , DHEIGHT *3 / 4, 80, 60, 28, 0,"Back");

    Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text

    Cmd_Text(DWIDTH / 4 , DHEIGHT / 4 - 2 * 24, 26, OPT_CENTER, "Engine oil reset"); //Write text in the center of the screen
    Cmd_Text(DWIDTH / 4 , DHEIGHT / 4 - 24, 26, OPT_CENTER, str); //Write text in the center of the screen

    Cmd_Text(DWIDTH * 3 / 4 , DHEIGHT / 4 - 2 * 24, 26, OPT_CENTER, "Mission oil reset"); //Write text in the center of the screen
    Cmd_Text(DWIDTH * 3 / 4 , DHEIGHT / 4 - 24 , 26, OPT_CENTER, str2); //Write text in the center of the screen

    Cmd_Text(DWIDTH / 2 , DHEIGHT *2 / 3 - 24, 26, OPT_CENTER, "Break oil reset"); //Write text in the center of the screen
    Cmd_Text(DWIDTH / 2 , DHEIGHT *2 / 3 - 24 *0.5, 26, OPT_CENTER, str3); //Write text in the center of the screen

    Send_CMD(DISPLAY());                     //End the display list
    Send_CMD(CMD_SWAP);                      //Swap commands into RAM
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
}

void display_reset_choice(void)
{

    Send_CMD(CMD_DLSTART);                   //Start a new display list
    Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
    Send_CMD(CLEAR_COLOR_RGB(100, 100, 100));      //Determine the clear screen color
    Send_CMD(CLEAR(1, 1, 1));                //Clear the screen and the curren display list

    Send_CMD(TAG(8));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH / 5 - 30 , DHEIGHT  / 2 , 100, 80, 28, 0,"Yes");
    Send_CMD(TAG(9));                        // Tag the blue dot with a touch ID
    Cmd_Button(DWIDTH  / 2 + 30 , DHEIGHT / 2, 100, 80, 28, 0,"No");

    Cmd_Text(DWIDTH / 2 , DHEIGHT / 2 - 2 * 24, 30, OPT_CENTER, "Want to reset?"); //Write text in the center of the screen


    Send_CMD(DISPLAY());                     //End the display list
    Send_CMD(CMD_SWAP);                      //Swap commands into RAM
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
}
