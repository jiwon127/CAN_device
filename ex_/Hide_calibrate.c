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

#define DEBUG_TRANSMISSION_CAN_ID 0x6b2
#define DEBUG_RPM_CAN_ID 0x6b2
#define DEBUG_ACCELERATION_CAN_ID 0x6b2

#define TRANSMISSION_CAN_ID 0x43f
#define RPM_CAN_ID 0x316
#define ACCELERATION_CAN_ID 0x440

int hib_flag, hib_count, timer2_flag;
volatile int speed, int_distance, Engine_int_distance, Mission_int_distance, Break_int_distance; // km/h
volatile double interval, distance, Engine_distance, Mission_distance, Break_distance;  //  km
volatile unsigned int IRQ_A1,IRQ_A2, IRQ_B1, IRQ_B2;
volatile uint32_t irq_status,can_flag, isr_flag = 0;


char distance_array[16]={0,};
char Engine_distance_array[16]={0,};
char Mission_distance_array[16]={0,};
char Break_distance_array[16]={0,};


int *speed_ptr;

uint32_t distance_data[sizeof(distance)]={0,};
uint32_t distance_data_clear[sizeof(distance)]={0,};

uint32_t Engine_distance_data[sizeof(Engine_distance)]={0,};
uint32_t Mission_distance_data[sizeof(Mission_distance)]={0,};
uint32_t Break_distance_data[sizeof(Break_distance)]={0,};

uint32_t i=0;
hib_count = 0;

CANDATA_T globalCanData[MAX_ID_NUM];


void EVE_cmd_setbase(uint32_t base)
{
    uint32_t ftAddress;
    volatile short garbage = 0;
    volatile short realData_array[4] = {0,};
    ftAddress = REG_CMDB_WRITE;

    spi_cs_enable();

    spi_send_byte( ( ftAddress >> 16) | 0x80);
    garbage = spi_receive_byte();
    spi_send_byte(  ftAddress >> 8 );
    garbage = spi_receive_byte();
    spi_send_byte( ftAddress);
    garbage = spi_receive_byte();

    spi_send_byte(0xFFFFFF38);
    garbage = spi_receive_byte();
    spi_send_byte(base);
    garbage = spi_receive_byte();

    spi_cs_disable();

}

int main()
{
    Sysctl_Clock();
    spi_init_conf();
    hib_init();
    SPI_LCD_ACTIVE();
    FT81x_Init();
    ClearScreen();

    spi_write_32(REG_TOUCH_TRANSFORM_A +RAM_REG, 0x000109E4);
    spi_write_32(REG_TOUCH_TRANSFORM_B +RAM_REG, 0x000007A6);
    spi_write_32(REG_TOUCH_TRANSFORM_C +RAM_REG, 0xFFEC1EBA);
    spi_write_32(REG_TOUCH_TRANSFORM_D +RAM_REG, 0x0000072C);
    spi_write_32(REG_TOUCH_TRANSFORM_E +RAM_REG, 0x0001096A);
    spi_write_32(REG_TOUCH_TRANSFORM_F +RAM_REG, 0xFFF469CF);


    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR_COLOR_RGB(0,0,0));
    Send_CMD(CLEAR(1,1,1));
    Cmd_Text(DWIDTH / 2, 50, 26, OPT_CENTER, "Please tap on the dot.");
    Cmd_Calibrate(0);
    Send_CMD(DISPLAY());
    Send_CMD(CMD_SWAP);
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO

    uint32_t touch_a, touch_b, touch_c, touch_d, touch_e, touch_f;

    touch_a = spi_read_32(REG_TOUCH_TRANSFORM_A+RAM_REG);
    touch_b = spi_read_32(REG_TOUCH_TRANSFORM_B+RAM_REG);
    touch_c = spi_read_32(REG_TOUCH_TRANSFORM_C+RAM_REG);
    touch_d = spi_read_32(REG_TOUCH_TRANSFORM_D+RAM_REG);
    touch_e = spi_read_32(REG_TOUCH_TRANSFORM_E+RAM_REG);
    touch_f = spi_read_32(REG_TOUCH_TRANSFORM_F+RAM_REG);

    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR_COLOR_RGB(0,0,0));
    Send_CMD(CLEAR(1,1,1));
    Send_CMD(TAG(0));

    Cmd_Text(5, 15, 26, 0, "TOUCH_TRANSFORM_A:");
    Cmd_Text(5, 30, 26, 0, "TOUCH_TRANSFORM_B:");
    Cmd_Text(5, 45, 26, 0, "TOUCH_TRANSFORM_C:");
    Cmd_Text(5, 60, 26, 0, "TOUCH_TRANSFORM_D:");
    Cmd_Text(5, 75, 26, 0, "TOUCH_TRANSFORM_E:");
    Cmd_Text(5, 90, 26, 0, "TOUCH_TRANSFORM_F:");

    EVE_cmd_setbase(16L);

    Cmd_Number(310, 15, 26, OPT_RIGHTX|8, touch_a);
    Cmd_Number(310, 30, 26, OPT_RIGHTX|8, touch_b);
    Cmd_Number(310, 45, 26, OPT_RIGHTX|8, touch_c);
    Cmd_Number(310, 60, 26, OPT_RIGHTX|8, touch_d);
    Cmd_Number(310, 75, 26, OPT_RIGHTX|8, touch_e);
    Cmd_Number(310, 90, 26, OPT_RIGHTX|8, touch_f);

    Send_CMD(DISPLAY()); /* instruct the graphics processor to show the list */
    Send_CMD(CMD_SWAP); /* make this list active */
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO

    while(1);
    return 1;
}
