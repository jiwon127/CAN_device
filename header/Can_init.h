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


void can_to_spi_rpm(char* (*a),unsigned int arg1);
void can_to_spi_acceleration(char* (*a),unsigned int arg1);
void can_to_spi_transmission(char* (*a),unsigned int arg1);

#define MAX_ID_NUM  3   //IF you want to get more can data, increase this.

typedef struct
{
    unsigned char CANDATA[8];
    unsigned char ifValid;
} CANDATA_T;

extern CANDATA_T globalCanData[MAX_ID_NUM];

#endif /* HEADER_CAN_INIT_H_ */
