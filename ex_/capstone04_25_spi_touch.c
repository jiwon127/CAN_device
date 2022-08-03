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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>


#define DISPLAY_70 1
#define DISPLAY_50 2
#define DISPLAY_43 3
#define DISPLAY_39 4
#define DISPLAY_38 5
#define DISPLAY_35 6
#define DISPLAY_29 7
#define DISPLAY_40 8
#define DISPLAY_101 9

#define BOARD_EVE2 1
#define BOARD_EVE3 2
#define BOARD_EVE4 3

#define TOUCH_TPN 0
#define TOUCH_TPR 1
#define TOUCH_TPC 2

int FT81x_Init_1(int display, int board, int touch);



int main()
{
    FT81x_Init_1(DISPLAY_101, BOARD_EVE3, TOUCH_TPN); //Initialize the EVE graphics controller.
    ClearScreen();                                  //Clear any remnants in the RAM
    if (Display_Touch() == TOUCH_TPR)
    {
        Calibrate();
    }

    MakeScreen_MatrixOrbital(30);                   //Draw the Matrix Orbital Screen
    uint8_t pressed = 0;

    while (1)
    {

        uint8_t Tag = spi_read_8(REG_TOUCH_TAG + RAM_REG);  //Check for touches
        switch (Tag)
        {
            case 1:
                if (!pressed)
                {
                    MakeScreen_MatrixOrbital(120); //Blue dot is 120 when touched
                    pressed = 1;
                }
                break;
            default:
                if (pressed)
                {
                    pressed = 0;
                    MakeScreen_MatrixOrbital(30); //Blue dot size is 30 when not touched
                }
                break;
        }
    }

    while(1){};

    return 0;
}



// Call this function once at powerup to reset and initialize the Eve chip
int FT81x_Init_1(int display, int board, int touch)
{
    uint32_t Ready = false;
    int Width = DWIDTH;
    int Height = DHEIGHT;
    int HOffset = PIXHOFFSET;
    int VOffset = PIXVOFFSET;
    int Touch = touch;

    Eve_Reset(); // Hard reset of the Eve chip

    // Wakeup Eve
    if (board >= BOARD_EVE3)
    {
        HostCommand(HCMD_CLKEXT);
    }
    HostCommand(HCMD_ACTIVE);
    delay_ms(300);

    do
    {
        Ready = Cmd_READ_REG_ID();
    } while (!Ready);

    //  Log("Eve now ACTIVE\n");         //

    Ready = spi_read_32(REG_CHIP_ID);
    uint16_t ValH = Ready >> 16;
    uint16_t ValL = Ready & 0xFFFF;


    if (display == DISPLAY_101)
    {
        spi_write_32(REG_FREQUENCY + RAM_REG, 80000000); // Configure the system clock to 80MHz
    }
    else
    {
        spi_write_32(REG_FREQUENCY + RAM_REG, 60000000); // Configure the system clock to 60MHz
    }
    // Before we go any further with Eve, it is a good idea to check to see if she is wigging out about something
    // that happened before the last reset.  If Eve has just done a power cycle, this would be unnecessary.
    if (spi_read_16(REG_CMD_READ + RAM_REG) == 0xFFF)
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
    spi_write_16(REG_HCYCLE + RAM_REG, HCYCLE);         // Set H_Cycle to 548
    spi_write_16(REG_HOFFSET + RAM_REG, HOFFSET);       // Set H_Offset to 43
    spi_write_16(REG_HSYNC0 + RAM_REG, HSYNC0);         // Set H_SYNC_0 to 0
    spi_write_16(REG_HSYNC1 + RAM_REG, HSYNC1);         // Set H_SYNC_1 to 41
    spi_write_16(REG_VCYCLE + RAM_REG, VCYCLE);         // Set V_Cycle to 292
    spi_write_16(REG_VOFFSET + RAM_REG, VOFFSET);       // Set V_OFFSET to 12
    spi_write_16(REG_VSYNC0 + RAM_REG, VSYNC0);         // Set V_SYNC_0 to 0
    spi_write_16(REG_VSYNC1 + RAM_REG, VSYNC1);         // Set V_SYNC_1 to 10
    spi_write_8(REG_SWIZZLE + RAM_REG, SWIZZLE);        // Set SWIZZLE to 0
    spi_write_8(REG_PCLK_POL + RAM_REG, PCLK_POL);      // Set PCLK_POL to 1
    spi_write_16(REG_HSIZE + RAM_REG, HSIZE);           // Set H_SIZE to 480
    spi_write_16(REG_VSIZE + RAM_REG, VSIZE);           // Set V_SIZE to 272
    spi_write_8(REG_CSPREAD + RAM_REG, CSPREAD);        // Set CSPREAD to 1    (32 bit register - write only 8 bits)
    spi_write_8(REG_DITHER + RAM_REG, DITHER);          // Set DITHER to 1     (32 bit register - write only 8 bits)

    // configure touch & audio
    if (touch == TOUCH_TPR)
    {
        spi_write_16(REG_TOUCH_CONFIG + RAM_REG, 0x8381);
    }
    else if (touch == TOUCH_TPC)
    {
        if (display == DISPLAY_40)
            spi_write_16(REG_TOUCH_CONFIG + RAM_REG, 0x480); // FT6336U
        else
            spi_write_16(REG_TOUCH_CONFIG + RAM_REG, 0x5d0);
        if (board == BOARD_EVE2)
        {
            Cap_Touch_Upload();
        }
    }

  spi_write_16(REG_TOUCH_RZTHRESH + RAM_REG, 1200);          // set touch resistance threshold
  spi_write_8(REG_TOUCH_MODE + RAM_REG, 0x02);               // set touch on: continous - this is default
  spi_write_8(REG_TOUCH_ADC_MODE + RAM_REG, 0x01);           // set ADC mode: differential - this is default
  spi_write_8(REG_TOUCH_OVERSAMPLE + RAM_REG, 15);           // set touch oversampling to max

  spi_write_16(REG_GPIOX_DIR + RAM_REG, 0x8000 | (1<<3));             // Set Disp GPIO Direction
  spi_write_16(REG_GPIOX + RAM_REG, 0x8000 | (1<<3));                 // Enable Disp (if used)

  spi_write_16(REG_PWM_HZ + RAM_REG, 0x00FA);                // Backlight PWM frequency
  spi_write_8(REG_PWM_DUTY + RAM_REG, 128);                  // Backlight PWM duty (on)

  // write first display list (which is a clear and blank screen)
  spi_write_32(RAM_DL+0, CLEAR_COLOR_RGB(0,0,0));
  spi_write_32(RAM_DL+4, CLEAR(1,1,1));
  spi_write_32(RAM_DL+8, DISPLAY());
  spi_write_8(REG_DLSWAP + RAM_REG, DLSWAP_FRAME);          // swap display lists
  spi_write_8(REG_PCLK + RAM_REG, PCLK);                       // after this display is visible on the LCD
  return 1;
}
