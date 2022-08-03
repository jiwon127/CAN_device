/*
 * my_eve2_lib2.h
 *
 *  Created on: 2021. 3. 3.
 *      Author: kjm-hsh
 */

#ifndef HEADER_MY_EVE2_LIB_H_
#define HEADER_MY_EVE2_LIB_H_

void calibrate_once(void);
void hongCalibrate(void);
void hongButton(int x, int y);

void make_gauge(void);
void make_character(unsigned char DotSize,const char* str);
void display_accel_trans_rpm(unsigned char DotSize,const char* str,const char* str2, const char* str3);
void display_fuel(unsigned char DotSize,const char* str,const char* str2, const char* str3);
void display_fuel_reset(unsigned char DotSize,const char* str,const char* str2, const char* str3);
void display_reset_choice(void);



#endif /* HEADER_MY_EVE2_LIB_H_ */
