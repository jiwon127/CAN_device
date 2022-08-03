#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Can_init.h>
#include <header/Spi_lcd.h>
#include <header/Delay.h>
#include <header/FT81X_LCD.h>
#include <header/eve2_lib.h>
#include <header/MatrixEve2Conf.h>
#include <header/Eve2_81x_2.h>            // Header for this file with prototypes, defines, and typedefs
#include <header/touch_cap_811.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>


#define WorkBuffSz 512

//extern uint8_t CTOUCH_CONFIG_DATA_G911[1216];

void FT81x_Init(void)
{

  spi_write_32(REG_FREQUENCY + RAM_REG, 0x1312d00); // Configure the system clock to 60MHz

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
    //if (touch == TOUCH_TPR)

      spi_write_16(REG_TOUCH_CONFIG + RAM_REG, 0x8381);

    //else if (touch == TOUCH_TPC)

        //spi_write_16(REG_TOUCH_CONFIG + RAM_REG, 0x5d0);
        //Cap_Touch_Upload();




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
  spi_write_8(REG_PCLK + RAM_REG, PCLK);       // after this display is visible on the LCD

}


void Cap_Touch_Upload(void)
{
    //---Goodix911 Configuration from AN336
    //Load the TOUCH_DATA_U8 or TOUCH_DATA_U32 array from file ��touch_cap_811.h�� via the FT81x command buffer RAM_CMD
    //uint8_t CTOUCH_CONFIG_DATA_G911[] = { TOUCH_DATA_U8 };

   // CoProWrCmdBuf(CTOUCH_CONFIG_DATA_G911, TOUCH_DATA_LEN);
    //Execute the commands till completion
    UpdateFIFO();
    Wait4CoProFIFOEmpty();
    //Hold the touch engine in reset(write REG_CPURESET = 2)
    spi_write_8(REG_CPU_RESET + RAM_REG, 2);
    //Set GPIO3 output LOW
    spi_write_8(REG_GPIOX_DIR + RAM_REG, (spi_read_8(RAM_REG + REG_GPIOX_DIR) | 0x08)); // Set Disp GPIO Direction
    spi_write_8(REG_GPIOX + RAM_REG, (spi_read_8(RAM_REG + REG_GPIOX) | 0xF7));         // Clear GPIO
    //Wait more than 100us
    delay_ms(1);
    //Write REG_CPURESET=0
    spi_write_8(REG_CPU_RESET + RAM_REG, 0);
    //Wait more than 55ms
    delay_ms(100);
    //Set GPIO3 to input (floating)
    spi_write_8(REG_GPIOX_DIR + RAM_REG, (spi_read_8(RAM_REG + REG_GPIOX_DIR) & 0xF7));             // Set Disp GPIO Direction
                                                                         //---Goodix911 Configuration from AN336
}

// *** CoProWrCmdBuf() - Transfer a buffer into the CoPro FIFO as part of an ongoing command operation ***********
void CoProWrCmdBuf(const uint8_t *buff, uint32_t count)
{
  uint32_t TransferSize = 0;
  int32_t Remaining = count; // signed

  do {
    // Here is the situation:  You have up to about a megabyte of data to transfer into the FIFO
    // Your buffer is LogBuf - limited to 64 bytes (or some other value, but always limited).
    // You need to go around in loops taking 64 bytes at a time until all the data is gone.
    //
    // Most interactions with the FIFO are started and finished in one operation in an obvious fashion, but
    // here it is important to understand the difference between Eve RAM registers and Eve FIFO.  Even though
    // you are in the middle of a FIFO operation and filling the FIFO is an ongoing task, you are still free
    // to write and read non-FIFO registers on the Eve chip.
    //
    // Since the FIFO is 4K in size, but the RAM_G space is 1M in size, you can not, obviously, send all
    // the possible RAM_G data through the FIFO in one step.  Also, since the Eve is not capable of updating
    // it's own FIFO pointer as data is written, you will need to intermittently tell Eve to go process some
    // FIFO in order to make room in the FIFO for more RAM_G data.
    Wait4CoProFIFO(WorkBuffSz);                            // It is reasonable to wait for a small space instead of firing data piecemeal

    if (Remaining > WorkBuffSz)                            // Remaining data exceeds the size of our buffer
      TransferSize = WorkBuffSz;                           // So set the transfer size to that of our buffer
    else
    {
      TransferSize = Remaining;                            // Set size to this last dribble of data
      TransferSize = (TransferSize + 3) & 0xFFC;           // 4 byte alignment
    }

    StartCoProTransfer(FifoWriteLocation + RAM_CMD, false);// Base address of the Command Buffer plus our offset into it - Start SPI transaction

    spi_writebuffer( TransferSize);         // write the little bit for which we found space
    buff += TransferSize;                                  // move the working data read pointer to the next fresh data

    FifoWriteLocation  = (FifoWriteLocation + TransferSize) % FT_CMD_FIFO_SIZE;
    spi_cs_disable();                                         // End SPI transaction with the FIFO

    spi_write_16(REG_CMD_WRITE + RAM_REG, FifoWriteLocation);      // Manually update the write position pointer to initiate processing of the FIFO
    Remaining -= TransferSize;                             // reduce what we want by what we sent

  }while (Remaining > 0);                                  // keep going as long as we still want more
}

// Find the space available in the GPU AKA CoProcessor AKA command buffer AKA FIFO
uint16_t CoProFIFO_FreeSpace(void)
{
  uint16_t cmdBufferDiff, cmdBufferRd, cmdBufferWr, retval;

  cmdBufferRd = spi_read_16(REG_CMD_READ + RAM_REG);
  cmdBufferWr = spi_read_16(REG_CMD_WRITE + RAM_REG);

  cmdBufferDiff = (cmdBufferWr-cmdBufferRd) % FT_CMD_FIFO_SIZE; // FT81x Programmers Guide 5.1.1
  retval = (FT_CMD_FIFO_SIZE - 4) - cmdBufferDiff;
  return (retval);
}

// Sit and wait until there are the specified number of bytes free in the <GPU/CoProcessor> incoming FIFO
void Wait4CoProFIFO(uint32_t room)
{
   uint16_t getfreespace;

   do {
     getfreespace = CoProFIFO_FreeSpace();
   }while(getfreespace < room);
}

// Every CoPro transaction starts with enabling the SPI and sending an address
void StartCoProTransfer(uint32_t address, uint8_t reading)
{
    spi_cs_enable();
  if (reading){
    spi_send_byte(address >> 16);
    spi_send_byte(address >> 8);
    spi_send_byte(address);
    spi_send_byte(0);
  }else{
    spi_send_byte((address >> 16) | 0x80);
    spi_send_byte(address >> 8);
    spi_send_byte(address);
  }
}








void Send_CMD(uint32_t data)
{
  spi_write_32(FifoWriteLocation + RAM_CMD, data); // write the command at the globally tracked "write pointer" for the FIFO

  FifoWriteLocation += FT_CMD_SIZE; // Increment the Write Address by the size of a command - which we just sent
  FifoWriteLocation %= FT_CMD_FIFO_SIZE; // Wrap the address to the FIFO space
}

void Cmd_Text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str)
{
  uint16_t DataPtr, LoopCount, StrPtr;

  uint16_t length = (uint16_t) strlen(str);
  if(!length)
    return;

  //uint32_t* data = (uint32_t*) calloc((length / 4) + 1, sizeof(uint32_t)); // Allocate memory for the string expansion
  uint32_t data[64]={0,};
  if (length >= sizeof(data)-1)
      return;

  StrPtr = 0;
  for(DataPtr=0; DataPtr<(length/4); ++DataPtr, StrPtr=StrPtr+4)
    data[DataPtr] = (uint32_t)str[StrPtr+3]<<24 | (uint32_t)str[StrPtr+2]<<16
                    | (uint32_t)str[StrPtr+1]<<8 | (uint32_t)str[StrPtr];

  for(LoopCount=0; LoopCount<(length%4); ++LoopCount, ++StrPtr)
    data[DataPtr] |= (uint32_t)str[StrPtr] << (LoopCount*8);

  // Set up the command
  Send_CMD(CMD_TEXT);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | font );

  // Send out the text
  for(LoopCount = 0; LoopCount <= length/4; LoopCount++)
    Send_CMD(data[LoopCount]);  // These text bytes get sucked up 4 at a time and fired at the FIFO

  //free(data);
}

void UpdateFIFO(void)
{
    spi_write_16(REG_CMD_WRITE + RAM_REG, FifoWriteLocation);               // We manually update the write position pointer
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

void ClearScreen(void)
{
    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));
    Send_CMD(CLEAR(1, 1, 1));
    Send_CMD(DISPLAY());
    Send_CMD(CMD_SWAP);
    UpdateFIFO();                            // Trigger the CoProcessor to start processing commands out of the FIFO
    Wait4CoProFIFOEmpty();                   // wait here until the coprocessor has read and executed every pending command.
    delay_ms(10);
}

void Wait4CoProFIFOEmpty(void)
{
  uint16_t ReadReg;
  uint8_t ErrChar;
  uint8_t buffy[2];

  do
  {
    ReadReg = spi_read_16(REG_CMD_READ + RAM_REG);
    if(ReadReg == 0xFFF)
    {
      // this is a error which would require sophistication to fix and continue but we fake it somewhat unsuccessfully
      uint8_t Offset = 0;
      do
      {
        // Get the error character and display it
        ErrChar = spi_read_8(RAM_ERR_REPORT + Offset);
        Offset++;
        sprintf(buffy, "%c", ErrChar);
      }while ( (ErrChar != 0) && (Offset < 128) ); // when the last stuffed character was null, we are done

      // Eve is unhappy - needs a paddling.
      uint32_t Patch_Add = spi_read_32(REG_COPRO_PATCH_PTR + RAM_REG);
      spi_write_8(REG_CPU_RESET + RAM_REG, 1);
      spi_write_8(REG_CMD_READ + RAM_REG, 0);
      spi_write_8(REG_CMD_WRITE + RAM_REG, 0);
      spi_write_8(REG_CMD_DL + RAM_REG, 0);
      spi_write_8(REG_CPU_RESET + RAM_REG, 0);
      spi_write_32(REG_COPRO_PATCH_PTR + RAM_REG, Patch_Add);
      delay_ms(250);  // we already saw one error message and we don't need to see then 1000 times a second
    }
  }while( ReadReg != spi_read_16(REG_CMD_WRITE + RAM_REG) );
}


void Calibrate_Manual(uint16_t Width, uint16_t Height, uint16_t V_Offset, uint16_t H_Offset)
{
  uint32_t displayX[3], displayY[3];
  uint32_t touchX[3], touchY[3];
  uint32_t touchValue = 0, storedValue = 0;
  int32_t tmp, k;
  int32_t TransMatrix[6];
  uint8_t count = 0;
  uint8_t pressed = 0;
  char num[2];

  // These values determine where your calibration points will be drawn on your display
  displayX[0] = (uint32_t) (Width * 0.15) + H_Offset;
  displayY[0] = (uint32_t) (Height * 0.15) + V_Offset;

  displayX[1] = (uint32_t) (Width * 0.85) + H_Offset;
  displayY[1] = (uint32_t) (Height / 2) + V_Offset;

  displayX[2] = (uint32_t) (Width / 2) + H_Offset;
  displayY[2] = (uint32_t) (Height * 0.85) + V_Offset;

  while (count < 3)
  {
    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR_COLOR_RGB(0, 0, 0));
    Send_CMD(CLEAR(1,1,1));

    // Draw Calibration Point on screen
    Send_CMD(COLOR_RGB(255, 0, 0));
    Send_CMD(POINT_SIZE(20 * 16));
    Send_CMD(BEGIN(POINTS));
    Send_CMD(VERTEX2F((uint32_t)(displayX[count]) * 16, (uint32_t)((displayY[count])) * 16));
    Send_CMD(END());
    Send_CMD(COLOR_RGB(255, 255, 255));
    Cmd_Text((Width / 2) + H_Offset, (Height / 3) + V_Offset, 27, OPT_CENTER, "Calibrating");
    Cmd_Text((Width / 2) + H_Offset, (Height / 2) + V_Offset, 27, OPT_CENTER, "Please tap the dots");
    num[0] = count + 0x31; num[1] = 0;                                            // null terminated string of one character
    Cmd_Text(displayX[count], displayY[count], 27, OPT_CENTER, num);

    Send_CMD(DISPLAY());
    Send_CMD(CMD_SWAP);
    UpdateFIFO();                             // Trigger the CoProcessor to start processing commands out of the FIFO
    Wait4CoProFIFOEmpty();                   // wait here until the coprocessor has read and executed every pending command.
    delay_ms(300);

    while (pressed == count)
    {
        touchValue = spi_read_32(REG_TOUCH_DIRECT_XY + RAM_REG);    // Read for any new touch tag inputs
        if (!(touchValue & 0x80000000))
        {
            touchX[count] = (touchValue >> 16) & 0x03FF;            // Raw Touchscreen Y coordinate
            touchY[count] = touchValue & 0x03FF;                    // Raw Touchscreen Y coordinate

                                //Log("\ndisplay x[%d]: %ld display y[%d]: %ld\n", count, displayX[count], count, displayY[count]);
                                //Log("touch x[%d]: %ld touch y[%d]: %ld\n", count, touchX[count], count, touchY[count]);

            count++;
        }
    }
    pressed = count;

  }

  k = ((touchX[0] - touchX[2])*(touchY[1] - touchY[2])) - ((touchX[1] - touchX[2])*(touchY[0] - touchY[2]));

  tmp = (((displayX[0] - displayX[2]) * (touchY[1] - touchY[2])) - ((displayX[1] - displayX[2])*(touchY[0] - touchY[2])));
  TransMatrix[0] = ((int64_t)tmp << 16) / k;

  tmp = (((touchX[0] - touchX[2]) * (displayX[1] - displayX[2])) - ((displayX[0] - displayX[2])*(touchX[1] - touchX[2])));
  TransMatrix[1] = ((int64_t)tmp << 16) / k;

  tmp = ((touchY[0] * (((touchX[2] * displayX[1]) - (touchX[1] * displayX[2])))) + (touchY[1] * (((touchX[0] * displayX[2]) - (touchX[2] * displayX[0])))) + (touchY[2] * (((touchX[1] * displayX[0]) - (touchX[0] * displayX[1])))));
  TransMatrix[2] = ((int64_t)tmp << 16) / k;

  tmp = (((displayY[0] - displayY[2]) * (touchY[1] - touchY[2])) - ((displayY[1] - displayY[2])*(touchY[0] - touchY[2])));
  TransMatrix[3] = ((int64_t)tmp << 16) / k;

  tmp = (((touchX[0] - touchX[2]) * (displayY[1] - displayY[2])) - ((displayY[0] - displayY[2])*(touchX[1] - touchX[2])));
  TransMatrix[4] = ((int64_t)tmp << 16) / k;

  tmp = ((touchY[0] * (((touchX[2] * displayY[1]) - (touchX[1] * displayY[2])))) + (touchY[1] * (((touchX[0] * displayY[2]) - (touchX[2] * displayY[0])))) + (touchY[2] * (((touchX[1] * displayY[0]) - (touchX[0] * displayY[1])))));
  TransMatrix[5] = ((int64_t)tmp << 16) / k;

  count = 0;
  do
  {
    spi_write_32(REG_TOUCH_TRANSFORM_A + RAM_REG + (count * 4), TransMatrix[count]);  // Write to Eve config registers

//    uint16_t ValH = TransMatrix[count] >> 16;
//    uint16_t ValL = TransMatrix[count] & 0xFFFF;
//    Log("TM%d: 0x%04x %04x\n", count, ValH, ValL);

    count++;
  }while(count < 6);
}


void Calibrate(void)
{
    Calibrate_Manual(DWIDTH, DHEIGHT, PIXVOFFSET, PIXHOFFSET);
}

