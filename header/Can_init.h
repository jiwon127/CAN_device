/*
 * Can_init.h
 *
 *  Created on: 2021. 2. 3.
 *      Author: kjm-hsh
 */

#ifndef HEADER_CAN_INIT_H_
#define HEADER_CAN_INIT_H_

void can_init(int bps, int irq);
void can_rx2(int id, char msgBoxNum);    //Purely setup Message Box.
void can_rx_conf(int id);
void can_tx_conf(int id);

void printReceivedCanData(int uartNum, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4 );
void CanToSpi(char* (*a),unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4);

void can_to_spi_rpm(char* (*a),unsigned int arg1);
void can_to_spi_acceleration(char* (*a),unsigned int arg1);
void can_to_spi_transmission(char* (*a),unsigned int arg1);

void can_to_spi_rpm2(int can_id, char* (*a));
void can_to_spi_acceleration2(int can_id, char* (*a));
void can_to_spi_transmission2(int can_id, char* (*a));

void check_wake_state(unsigned int arg1);

#define MAX_ID_NUM  3   //IF you want to get more can data, increase this.

typedef struct
{
    unsigned char CANDATA[8];
    unsigned char ifValid;
} CANDATA_T;

extern CANDATA_T globalCanData[MAX_ID_NUM];

#endif /* HEADER_CAN_INIT_H_ */
