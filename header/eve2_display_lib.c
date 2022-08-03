/*
 * eve2_display_lib.c
 *
 *  Created on: 2021. 5. 7.
 *      Author: kjm-hsh
 */

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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>


// ******************** Screen Object Creation CoProcessor Command Functions ******************************

// *** Draw Slider - FT81x Series Programmers Guide Section 5.38 *************************************************
void Cmd_Slider(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t options, uint16_t val, uint16_t range)
{
  Send_CMD(CMD_SLIDER);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)h << 16) | w );
  Send_CMD( ((uint32_t)val << 16) | options );
  Send_CMD( (uint32_t)range );
}

// *** Draw Spinner - FT81x Series Programmers Guide Section 5.54 *************************************************
void Cmd_Spinner(uint16_t x, uint16_t y, uint16_t style, uint16_t scale)
{
  Send_CMD(CMD_SPINNER);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)scale << 16) | style );
}

// *** Draw Gauge - FT81x Series Programmers Guide Section 5.33 **************************************************
void Cmd_Gauge(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t major, uint16_t minor, uint16_t val, uint16_t range)
{
  Send_CMD(CMD_GAUGE);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | r );
  Send_CMD( ((uint32_t)minor << 16) | major );
  Send_CMD( ((uint32_t)range << 16) | val );
}

// *** Draw Dial - FT81x Series Programmers Guide Section 5.39 **************************************************
// This is much like a Gauge except for the helpful range parameter.  For some reason, all dials are 65535 around.
void Cmd_Dial(uint16_t x, uint16_t y, uint16_t r, uint16_t options, uint16_t val)
{
  Send_CMD(CMD_DIAL);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | r );
  Send_CMD( (uint32_t)val );
}

// *** Make Track (for a slider) - FT81x Series Programmers Guide Section 5.62 ************************************
// tag refers to the tag # previously assigned to the object that this track is tracking.
void Cmd_Track(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t tag)
{
    Send_CMD(CMD_TRACK);
    Send_CMD( ((uint32_t)y << 16) | x );
    Send_CMD( ((uint32_t)h << 16) | w );
    Send_CMD( (uint32_t)tag );
}

// *** Draw Number - FT81x Series Programmers Guide Section 5.43 *************************************************
void Cmd_Number(uint16_t x, uint16_t y, uint16_t font, uint16_t options, uint32_t num)
{
  Send_CMD(CMD_NUMBER);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | font );
  Send_CMD(num);
}

// *** Draw Smooth Color Gradient - FT81x Series Programmers Guide Section 5.34 **********************************
void Cmd_Gradient(uint16_t x0, uint16_t y0, uint32_t rgb0, uint16_t x1, uint16_t y1, uint32_t rgb1)
{
  Send_CMD(CMD_GRADIENT);
  Send_CMD( ((uint32_t)y0<<16)|x0 );
  Send_CMD(rgb0);
  Send_CMD( ((uint32_t)y1<<16)|x1 );
  Send_CMD(rgb1);
}

// *** Draw Button - FT81x Series Programmers Guide Section 5.28 **************************************************
void Cmd_Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t font, uint16_t options, const char* str)
{
  uint16_t DataPtr, LoopCount, StrPtr;

  uint16_t length = (uint16_t)strlen(str);
  if(!length)
    return;

  uint32_t* data = (uint32_t*) calloc((length/4)+1, sizeof(uint32_t));

  StrPtr = 0;
  for(DataPtr=0; DataPtr<(length/4); DataPtr++, StrPtr += 4)
    data[DataPtr] = (uint32_t)str[StrPtr+3]<<24 | (uint32_t)str[StrPtr+2]<<16 | (uint32_t)str[StrPtr+1]<<8 | (uint32_t)str[StrPtr];

  for(LoopCount=0; LoopCount<(length%4); LoopCount++, StrPtr++)
    data[DataPtr] |= (uint32_t)str[StrPtr] << (LoopCount * 8);

  Send_CMD(CMD_BUTTON);
  Send_CMD( ((uint32_t)y << 16) | x ); // Put two 16 bit values together into one 32 bit value - do it little endian
  Send_CMD( ((uint32_t)h << 16) | w );
  Send_CMD( ((uint32_t)options << 16) | font );

  for (LoopCount = 0; LoopCount <= length / 4; LoopCount++)
  {
      Send_CMD(data[LoopCount]);
  }

  free(data);
}


// ******************** Miscellaneous Operation CoProcessor Command Functions ******************************

// *** Cmd_SetBitmap - generate DL commands for bitmap parms - FT81x Series Programmers Guide Section 5.65 *******
void Cmd_SetBitmap(uint32_t addr, uint16_t fmt, uint16_t width, uint16_t height)
{
  Send_CMD( CMD_SETBITMAP );
  Send_CMD( addr );
  Send_CMD( ((uint32_t)width << 16) | fmt );
  Send_CMD( (uint32_t)height);
}

// *** Cmd_Memcpy - background copy a block of data - FT81x Series Programmers Guide Section 5.27 ****************
void Cmd_Memcpy(uint32_t dest, uint32_t src, uint32_t num)
{
  Send_CMD(CMD_MEMCPY);
  Send_CMD(dest);
  Send_CMD(src);
  Send_CMD(num);
}

// *** Cmd_GetPtr - Get the last used address from CoPro operation - FT81x Series Programmers Guide Section 5.47 *
void Cmd_GetPtr(void)
{
  Send_CMD(CMD_GETPTR);
  Send_CMD(0);
}

// *** Set Highlight Gradient Color - FT81x Series Programmers Guide Section 5.32 ********************************
void Cmd_GradientColor(uint32_t c)
{
  Send_CMD(CMD_GRADCOLOR);
  Send_CMD(c);
}

// *** Set FG color - FT81x Series Programmers Guide Section 5.30 ************************************************
void Cmd_FGcolor(uint32_t c)
{
  Send_CMD(CMD_FGCOLOR);
  Send_CMD(c);
}

// *** Set BG color - FT81x Series Programmers Guide Section 5.31 ************************************************
void Cmd_BGcolor(uint32_t c)
{
  Send_CMD(CMD_BGCOLOR);
  Send_CMD(c);
}

// *** Translate Matrix - FT81x Series Programmers Guide Section 5.51 ********************************************
void Cmd_Translate(uint32_t tx, uint32_t ty)
{
  Send_CMD(CMD_TRANSLATE);
  Send_CMD(tx);
  Send_CMD(ty);
}

// *** Rotate Matrix - FT81x Series Programmers Guide Section 5.50 ***********************************************
void Cmd_Rotate(uint32_t a)
{
  Send_CMD(CMD_ROTATE);
  Send_CMD(a);
}

// *** Rotate Screen - FT81x Series Programmers Guide Section 5.53 ***********************************************
void Cmd_SetRotate(uint32_t rotation)
{
  Send_CMD(CMD_SETROTATE);
  Send_CMD(rotation);
}

// *** Scale Matrix - FT81x Series Programmers Guide Section 5.49 ************************************************
void Cmd_Scale(uint32_t sx, uint32_t sy)
{
  Send_CMD(CMD_SCALE);
  Send_CMD(sx);
  Send_CMD(sy);
}

void Cmd_Flash_Fast(void)
{
  Send_CMD(CMD_FLASHFAST);
  Send_CMD(0);
}

// *** Calibrate Touch Digitizer - FT81x Series Programmers Guide Section 5.52 ***********************************
// * This business about "result" in the manual really seems to be simply leftover cruft of no purpose - send zero
void Cmd_Calibrate(uint32_t result)
{
  Send_CMD(CMD_CALIBRATE);
  Send_CMD(result);
}


// ***************************************************************************************************************
// *** Animation functions ***************************************************************************************
// ***************************************************************************************************************

void Cmd_AnimStart(int32_t ch, uint32_t aoptr, uint32_t loop)
{
    Send_CMD(CMD_ANIMSTART);
    Send_CMD(ch);
    Send_CMD(aoptr);
    Send_CMD(loop);
}

void Cmd_AnimStop(int32_t ch)
{
    Send_CMD(CMD_ANIMSTOP);
    Send_CMD(ch);
}

void Cmd_AnimXY(int32_t ch, int16_t x, int16_t y)
{
    Send_CMD(CMD_ANIMXY);
    Send_CMD(ch);
    Send_CMD(((uint32_t)y << 16) | x);
}

void Cmd_AnimDraw(int32_t ch)
{
    Send_CMD(CMD_ANIMDRAW);
    Send_CMD(ch);
}

void Cmd_AnimDrawFrame(int16_t x, int16_t y, uint32_t aoptr, uint32_t frame)
{
    Send_CMD(CMD_ANIMFRAME);
    Send_CMD(((uint32_t)y << 16) | x);
    Send_CMD(aoptr);
    Send_CMD(frame);
}
