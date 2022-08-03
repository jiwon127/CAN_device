/*
 * distance.c
 *
 *  Created on: 2021. 5. 4.
 *      Author: kjm-hsh
 */

#include <header/Flash_memory.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern uint32_t Engine_distance_data[8];
extern uint32_t Mission_distance_data[8];
extern uint32_t Break_distance_data[8];


void Save_Distance_10km(const char* str)
{
    uint32_t BeforeDistance = 0;
    BeforeDistance = (uint32_t)str[4];

    if( BeforeDistance != str[4] )
    {
        Flash_erase(0,1);
        Flash_write(Engine_distance_data,4,0);
        Flash_erase(64,65);
        Flash_write(Mission_distance_data,4,256);
        Flash_erase(128,129);
        Flash_write(Break_distance_data,4,512);
    }
}

