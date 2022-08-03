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

void MakeScreen_MatrixOrbital(uint8_t DotSize);
void FT81x_Init(void);

int main()
{
    Sysctl_Clock();
    spi_init_conf();
    uart_init();
    uart_tx_str("Hello can start!!\r\n");

    SPI_LCD_ACTIVE();
    //SPI_LCD_CLKEXT();


    uart_tx_str("first\r\n");

    FT81x_Init();
    MakeScreen_MatrixOrbital(80);


    while(1){}
}


void FT81x_Init(void)
{

  spi_write_32(REG_FREQUENCY + RAM_REG, 0x3938700); // Configure the system clock to 60MHz

  if( spi_read_16(REG_CMD_READ + RAM_REG) == 0xFFF )
  {
    // Eve is unhappy - needs a paddling.
    uint32_t Patch_Add = spi_read_32(REG_COPRO_PATCH_PTR + RAM_REG);
    spi_write_8(REG_CPU_RESET + RAM_REG, 1);
    spi_write_16(REG_CMD_READ + RAM_REG, 0);
    spi_write_16(REG_CMD_WRITE + RAM_REG, 0);
    spi_write_16(REG_CMD_DL + RAM_REG, 0);
    spi_write_8(REG_CPU_RESET + RAM_REG, 0);
    spi_write_32(REG_COPRO_PATCH_PTR + RAM_REG, Patch_Add);
  }

  // turn off screen output during startup
  spi_write_8(REG_GPIOX + RAM_REG, 0);             // Set REG_GPIOX to 0 to turn off the LCD DISP signal
  spi_write_8(REG_PCLK + RAM_REG, 0);              // Pixel Clock Output disable

  // load parameters of the physical screen to the Eve
  // All of these registers are 32 bits, but most bits are reserved, so only write what is actually used
  spi_write_16(REG_HCYCLE + RAM_REG, 548);         // Set H_Cycle to 548
  spi_write_16(REG_HOFFSET + RAM_REG, 43);       // Set H_Offset to 43
  spi_write_16(REG_HSYNC0 + RAM_REG, 0);         // Set H_SYNC_0 to 0
  spi_write_16(REG_HSYNC1 + RAM_REG, 41);         // Set H_SYNC_1 to 41
  spi_write_16(REG_VCYCLE + RAM_REG, 292);         // Set V_Cycle to 292
  spi_write_16(REG_VOFFSET + RAM_REG, 12);       // Set V_OFFSET to 12
  spi_write_16(REG_VSYNC0 + RAM_REG, 0);         // Set V_SYNC_0 to 0
  spi_write_16(REG_VSYNC1 + RAM_REG, 10);         // Set V_SYNC_1 to 10
  spi_write_8(REG_SWIZZLE + RAM_REG, 0);        // Set SWIZZLE to 0
  spi_write_8(REG_PCLK_POL + RAM_REG, 1);      // Set PCLK_POL to 1
  spi_write_16(REG_HSIZE + RAM_REG, 480);           // Set H_SIZE to 480
  spi_write_16(REG_VSIZE + RAM_REG, 272);           // Set V_SIZE to 272
  spi_write_8(REG_CSPREAD + RAM_REG, 1);        // Set CSPREAD to 1    (32 bit register - write only 8 bits)
  spi_write_8(REG_DITHER + RAM_REG, 1);          // Set DITHER to 1     (32 bit register - write only 8 bits)


  spi_write_16(REG_TOUCH_RZTHRESH + RAM_REG, 1200);          // set touch resistance threshold
  spi_write_8(REG_TOUCH_MODE + RAM_REG, 0x02);               // set touch on: continous - this is default
  spi_write_8(REG_TOUCH_ADC_MODE + RAM_REG, 0x01);           // set ADC mode: differential - this is default
  spi_write_8(REG_TOUCH_OVERSAMPLE + RAM_REG, 15);           // set touch oversampling to max

  spi_write_16(REG_GPIOX_DIR + RAM_REG, 0x8000);             // Set Disp GPIO Direction
  spi_write_16(REG_GPIOX + RAM_REG, 0x8000);                 // Enable Disp (if used)

  spi_write_16(REG_PWM_HZ + RAM_REG, 0x00FA);                // Backlight PWM frequency
  spi_write_8(REG_PWM_DUTY + RAM_REG, 128);                  // Backlight PWM duty (on)

  // write first display list (which is a clear and blank screen)
  spi_write_32(RAM_DL+0, CLEAR_COLOR_RGB(0,0,0));
  spi_write_32(RAM_DL+4, CLEAR(1,1,1));
  spi_write_32(RAM_DL+8, DISPLAY());
  spi_write_8(REG_DLSWAP + RAM_REG, DLSWAP_FRAME);          // swap display lists
  spi_write_8(REG_PCLK + RAM_REG, 5);       // after this display is visible on the LCD

}


void MakeScreen_MatrixOrbital(uint8_t DotSize)
{
    Send_CMD(CMD_DLSTART);                   //Start a new display list
    Send_CMD(VERTEXFORMAT(0));               //setup VERTEX2F to take pixel coordinates
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));      //Determine the clear screen color
    Send_CMD(CLEAR(1, 1, 1));                //Clear the screen and the curren display list
    Send_CMD(COLOR_RGB(0, 0, 192));        // change colour to blue
    Send_CMD(POINT_SIZE(DotSize * 16));      // set point size to DotSize pixels. Points = (pixels x 16)
    Send_CMD(BEGIN(POINTS));                 // start drawing point
    Send_CMD(TAG(1));                        // Tag the blue dot with a touch ID
    Send_CMD(VERTEX2F(DWIDTH / 2, DHEIGHT / 2));     // place blue point
    Send_CMD(END());                         // end drawing point
    Send_CMD(COLOR_RGB(255, 255, 255));      //Change color to white for text
    Cmd_Text(DWIDTH / 2, DHEIGHT / 2, 30, OPT_CENTER, " MATRIX         ORBITAL"); //Write text in the center of the screen
    Send_CMD(DISPLAY());                     //End the display list
    Send_CMD(CMD_SWAP);                      //Swap commands into RAM
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
}




