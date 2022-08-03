#include <tm4c123gh6pm.h>
#include <stdio.h>
#include <stdint.h>

#include "Flash_memory.h"
#include<header/Delay.h>

extern volatile double interval, distance, Engine_distance, Mission_distance, Break_distance;  //  km
// yy
extern volatile double Engine_distance2, Mission_distance2, Break_distance2;  //  km
// yy
extern volatile double Engine_interval, Mission_interval, Break_interval;

extern uint32_t distance_data_clear[8];
extern uint32_t Engine_distance_data[sizeof(Engine_distance2)];
extern uint32_t Mission_distance_data[sizeof(Mission_distance2)];
extern uint32_t Break_distance_data[sizeof(Break_distance2)];
//yy
void Flash_erase(uint32_t  start, uint32_t  end)
{
    uint32_t  i;

    for(i=start; i< end; i++)
    {
        FLASH_FMA_R = FLASH_BASE_ADDR + ( i *4 );
        FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_ERASE ;
        while (FLASH_FMC_R & FLASH_FMC_ERASE) ;
    }
}   //erase each two byte (from start to end)

void Flash_erase_all()
{
    FLASH_FMA_R = FLASH_BASE_ADDR ;
    FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_MERASE ;
    while (FLASH_FMC_R & FLASH_FMC_ERASE) ;

}   //erase all flash memory


void Flash_write(const uint32_t  *data, uint32_t  size, uint32_t  start)
{
    uint32_t  i;
    for(i=0; i< size; i++)
    {
        FLASH_FMD_R = data[i];
        FLASH_FMA_R =  FLASH_BASE_ADDR + start + i ;
        FLASH_FMC_R = FLASH_FMC_WRKEY | FLASH_FMC_WRITE ;
        while (FLASH_FMC_R & FLASH_FMC_WRITE) ;
    }
}   //write each two byte (from start to end)

void Flash_read(uint32_t * data,uint32_t  size, uint32_t  start)
{
    uint32_t  i;
    for(i=0;i<size;i++)
    {
        data[i] = *(FLASH_BASE_ADDR + start + i);
        //read
    }
}   //read each two byte (from start to end)

void Flash_reset(uint32_t data)
{
    if(data == 0)
    {
        Flash_erase(0,63);
        Flash_write(distance_data_clear,4,0);
    }
    else if(data == 1)
    {
        Flash_erase(64,127);
        Flash_write(distance_data_clear,4,256);
    }
    else if(data == 2)
    {
        Flash_erase(128,255);
        Flash_write(distance_data_clear,4,512);
    }

}

void Flash_save(void)
{
    Flash_erase(0,1);
    Flash_write(Engine_distance_data,4,0);
    delay_ms(100);

    Flash_erase(64,65);
    Flash_write(Mission_distance_data,4,256);
    delay_ms(100);

    Flash_erase(128,129);
    Flash_write(Break_distance_data,4,512);
}


/*
erase each 64
write each 256
Flash_erase(0,63);
Flash_write(distance_data_clear,4,0);
Flash_erase(64,127);
Flash_write(distance_data_clear,4,256);
Flash_erase(128,255);
Flash_write(distance_data_clear,4,512);

Flash_read( Engine_distance_data,  4, 0);
Flash_read( Mission_distance_data,  4, 256);
Flash_read( Break_distance_data,  4, 512);
*/
