/*
 * Can_init.c
 *
 *  Created on: 2021. 2. 3.
 *      Author: kjm-hsh
 */

#include <header/Gpio_led.h>
#include <header/Sysctl_clk.h>
#include <header/Uart.h>
#include <header/Uart7.h>
#include <header/Delay.h>
#include <header/my_eve2_lib.h>
#include <header/Can_init.h>


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <tm4c123gh6pm.h>

extern volatile unsigned int IRQ_A1,IRQ_A2, IRQ_B1, IRQ_B2;
extern volatile uint32_t irq_status,can_flag;
volatile uint32_t sts_test;
extern volatile uint32_t i;
extern volatile uint32_t isr_flag ;
extern int can_check;

void can1_handler(void)
{
    int i=0;
    int bitCnt = 1;

//    sts_test = CAN1_STS_R;
    for (i=0; i<MAX_ID_NUM; i++)
    {
        if (CAN1_MSG1INT_R & bitCnt)
        {
            CAN1_IF1MCTL_R = (CAN1_IF1MCTL_R&(~(unsigned long)CAN_IF1MCTL_INTPND));
            CAN1_IF1CMSK_R =  CAN_IF1CMSK_CLRINTPND | CAN_IF1CMSK_NEWDAT | CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB ;

            CAN1_IF1CRQ_R = (CAN1_IF1CRQ_R&(~(unsigned long)CAN_IF1CRQ_MNUM_M)) | (i+1);

            while (1)
            {
                if (!(CAN1_IF1CRQ_R & CAN_IF1CRQ_BUSY))    //If not busy
                    break;
            }

            (*(short *)(&(globalCanData[i].CANDATA[0])) ) = CAN1_IF1DA1_R;
            (*(short *)(&(globalCanData[i].CANDATA[2])) ) = CAN1_IF1DA2_R;
            (*(short *)(&(globalCanData[i].CANDATA[4])) ) = CAN1_IF1DB1_R;
            (*(short *)(&(globalCanData[i].CANDATA[6])) ) = CAN1_IF1DB2_R;
            globalCanData[i].ifValid = 1;
        }
        bitCnt = bitCnt << 1;
    }
    irq_status = CAN1_STS_R;
}


void can_init(int bps, int irq)
{
    SYSCTL_RCGCCAN_R   = (SYSCTL_RCGCCAN_R&(~(unsigned long)(SYSCTL_RCGCCAN_R0|SYSCTL_RCGCCAN_R1)))
                            | (SYSCTL_RCGCCAN_R0|SYSCTL_RCGCCAN_R1);
    //CAN0, 1 CLOCK ON(1,0 <<1) == Enable CAN0, CAN1

    SYSCTL_RCGCGPIO_R  = (SYSCTL_RCGCGPIO_R&(~(unsigned long)SYSCTL_RCGCGPIO_R0))
                            | SYSCTL_RCGCGPIO_R0;
    //GPIO PORT A CLOCK ON

    GPIO_PORTA_DEN_R  = (GPIO_PORTA_DEN_R&(~(unsigned long)0x03)) | 0x01|0x02;
    //GPIO_PORTA_DEN_R = 0x000000FF;
    //use the pin as a digital input or output (either GPIO or alternate function), the corresponding GPIODEN bit must be set.

    GPIO_PORTA_AFSEL_R = (GPIO_PORTA_AFSEL_R&(~(unsigned long)0x03)) | 0x01|0x02;   //Pin0, Pin1
    //PORT A USE ALT FUNC CAN (PA0->1, PA1->1);


    GPIO_PORTA_PCTL_R  = (GPIO_PORTA_PCTL_R&(~(unsigned long)0xFF)) | 0x88;
    //GPIO_PORTA_PCTL_R =  0x******00 | 0x88-> 0x******88;
    //PA0->CANRx, PA1->CANTx

    CAN1_CTL_R = (CAN1_CTL_R&(~(unsigned long)CAN_CTL_INIT)) | CAN_CTL_INIT;
    //CAN INITIALIZE (put into initialize mode)
    CAN1_CTL_R = (CAN1_CTL_R&(~(unsigned long)CAN_CTL_CCE)) | CAN_CTL_CCE;    //CCE BIT.
    //CAN Register change enabled.

    if(irq == 1)
    {
        CAN1_CTL_R |= (CAN1_CTL_R&(~(unsigned long)( CAN_CTL_SIE| CAN_CTL_IE))) |
                        ( CAN_CTL_SIE | CAN_CTL_IE );

        NVIC_PRI10_R |= 0x00000080; // pri 10
        NVIC_EN1_R = 1<<8; // IRQ
    }
    //CAN Interrupt enable


    //CAN CLOCK: 20Mhz.
    //NOW systemClock = 20Mhz!->We use CAN Clock as 20Mhz, Let us calculate.
    //Note that "The fsys input clock is the system clock frequency as configured by the RCC or RCC2 registers"


    //We set 1Mhz, bitTime=1us, n=4 therefore, tq= 1us/4 = 250ns
    //tq = 250ns
    //tq = 500ns when 500Kbps, 2us, n=4, tq=2/4=500ns

    //BRP = 250ns * 20Mhz = 5
    //500kbps BRP = 5*2;

    //Below are relative value of tQ: no need to change (250->500)
    //tSync = 1*tq = 250ns(->500 when 500kbps)
    //tProp = 1*tq = 250ns(->same as above)

    //bittime = 1us = tSync+Tseg1 +Tseg2 = 4tq
    //tProp+tPhase1+tPhase2 = 3tq.
    //tPhase1= 1tQ.
    //tPahse2 = 1tq.

    //tSeg1 = 2tq,
    //tSe2 = 1tq.

    //TSEG2 = 1-1, TESG1 = 2-1, SJW = 1-1, BRP = 5-1 (10-1 when 500kbps)
    //0<<12, 1<<8,
    if(bps==1000)   CAN1_BIT_R = (CAN1_BIT_R&(~(unsigned long)0xFFFF)) | ( (0<<12) | (1<<8) | (0<<6) | ((5-1)<<0) );
    else if(bps==500) CAN1_BIT_R = (CAN1_BIT_R&(~(unsigned long)0xFFFF)) | ( (0<<12) | (1<<8) | (0<<6) | ((10-1)<<0) );

    CAN1_BRPE_R = 0;    //BPE is enough.
    CAN1_CTL_R = (CAN1_CTL_R&(~(unsigned long)CAN_CTL_CCE));    //Now no body can change bit rate..
    //Now we have clean CAN clock from here.

    CAN1_CTL_R = (CAN1_CTL_R&(~(unsigned long)CAN_CTL_INIT));   //Put into Running Mode.
}

void can_rx2(int id, char msgBoxNum)    //Purely setup Message Box.
{
    CAN1_IF1CMSK_R = (CAN1_IF1CMSK_R&(~(unsigned long)0xFF)) |
                           ( CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_MASK | CAN_IF1CMSK_ARB
                           | CAN_IF1CMSK_CONTROL  | CAN_IF1CMSK_CLRINTPND| CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB);
    CAN1_IF1MSK1_R = 0x0;
    CAN1_IF1MSK2_R = 0x1FFF;
    CAN1_IF1ARB2_R = (CAN1_IF1ARB2_R&(~(unsigned long)0xFFFF)) |
                ( CAN_IF1ARB2_MSGVAL | id << 2 );

    CAN1_IF1MCTL_R = (CAN1_IF1MCTL_R&(~(unsigned long)0xFFFF)) |
                ( //CAN_IF1MCTL_INTPND|
                        CAN_IF1MCTL_RXIE | CAN_IF1MCTL_UMASK
                        | CAN_IF1MCTL_EOB | 8 );
    CAN1_IF1CRQ_R  = (CAN1_IF1CRQ_R&(~(unsigned long)CAN_IF1CRQ_MNUM_M)) | msgBoxNum;

    while (1)
     {
         if (!(CAN1_IF1CRQ_R & CAN_IF1CRQ_BUSY))    //If not busy
             break;
     }

}

void can_rx_conf(int id)
{
    //Message Object configuration.
    //1.
    CAN1_IF1CMSK_R = (CAN1_IF1CMSK_R&(~(unsigned long)0xFF)) |
                        ( CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_MASK | CAN_IF1CMSK_ARB
                        | CAN_IF1CMSK_CONTROL | CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB);
    //MASK,ARB,CONTROL,CL~PND,NEWDAT,TXRQST,DATAA,DATAB ON

    //2-3. Set Mask
    CAN1_IF1MSK1_R = 0x0;
    CAN1_IF1MSK2_R = 0x1FFF;


    //4. Set Message ID.
    //CAN1_IF1ARB1_R = (CAN1_IF1ARB1_R&(~(unsigned long)0xFFFF)) | 0x01;
    //Message ID = 0x3B

    CAN1_IF1ARB2_R = (CAN1_IF1ARB2_R&(~(unsigned long)0xFFFF)) |
            ( CAN_IF1ARB2_MSGVAL | id << 2 );

    //MAGVAL, DIR Receive ON

    //5.
    CAN1_IF1MCTL_R = (CAN1_IF1MCTL_R&(~(unsigned long)0xFFFF)) |
                (  CAN_IF1MCTL_UMASK | CAN_IF1MCTL_EOB | 8 );
    // EOB ON , DATA LENGTH 8

    //6.
    //Message Object NUM = 02
    //Store data in IF1DAR, DA2, DB1, DB2 as soon as here.
    //We have to wait 3-6 CAN CLOCK Wait then Busybit == 0 Ok
    CAN1_IF1CRQ_R  = (CAN1_IF1CRQ_R&(~(unsigned long)CAN_IF1CRQ_MNUM_M)) | 0x04;

    while (1)
     {
         if (!(CAN1_IF1CRQ_R & CAN_IF1CRQ_BUSY))    //If not busy
             break;
         //busy->wait... may be 3-6 clock
     }

    //Wait for each valid msg ...
    while (1)
    {
        if (CAN1_NWDA1_R & (0x1 << (4-1)))
            break;
        else
            ;   //Wait.. until valid message comes.
    }

    CAN1_IF1CMSK_R = (CAN1_IF1CMSK_R&(~(unsigned long)CAN_IF1CMSK_WRNRD)) ;
    CAN1_IF1CRQ_R  = (CAN1_IF1CRQ_R&(~(unsigned long)CAN_IF1CRQ_MNUM_M)) | 0x04;

    while (1)
     {
         if (!(CAN1_IF1CRQ_R & CAN_IF1CRQ_BUSY))    //If not busy
             break;
         //busy->wait... may be 3-6 clock
     }
    CAN1_NWDA1_R = CAN1_NWDA1_R& (~(0x1 << (4-1)));
}


void can_tx_conf(int id)
{
    //Message Object configuration.

      //1.
      //WRNRD,MASK,ARB,CONTROL,CL~PND,NEWDAT,TXRQST,DATAA,DATAB ON
      CAN1_IF1CMSK_R = (CAN1_IF1CMSK_R&(~(unsigned long)0xFF)) |
                          ( CAN_IF1CMSK_WRNRD  | CAN_IF1CMSK_MASK | CAN_IF1CMSK_ARB
                          | CAN_IF1CMSK_CONTROL | CAN_IF1CMSK_CLRINTPND | CAN_IF1CMSK_NEWDAT
                          |   CAN_IF1CMSK_TXRQST | CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB);
      //2-3.
      //ID MASK X -> NO ID FILLTERLING
      //All ID.
      CAN1_IF1MSK1_R = 0x00;
      CAN1_IF1MSK2_R = 0x00;

      //4-5. Set Message ID.
      //CREATE MSG ID = 0x01
      //CAN1_IF1ARB1_R = (CAN1_IF1ARB1_R&(~(unsigned long)0xFFFF)) | 0x01;

      //MAGVAL, DIR Transmit, XTD ON
      CAN1_IF1ARB2_R = (CAN1_IF1ARB2_R&(~(unsigned long)0xFFFF)) |
              ( CAN_IF1ARB2_MSGVAL | CAN_IF1ARB2_DIR | id <<2);

      //6.
      //TXIE, EOB ON , DATA LENGTH 8
      CAN1_IF1MCTL_R = (CAN1_IF1MCTL_R&(~(unsigned long)0xFFF)) |
              ( CAN_IF1MCTL_NEWDAT | CAN_IF1MCTL_TXRQST | CAN_IF1MCTL_EOB | 8 );


      //7.
      //CAN DATA = 34 12 78 56 BC 9A F0 DE
      CAN1_IF1DA1_R  = 0xAAAA;
      CAN1_IF1DA2_R  = 0xBBBB;
      CAN1_IF1DB1_R  = 0xCCCC;
      CAN1_IF1DB2_R  = 0xDDDD;

      //8.
      //Message NUM = 1 //Copy IF1DAR, DA2, DB1, DB2 as soon as here.
      CAN1_IF1CRQ_R  = (CAN1_IF1CRQ_R&(~(unsigned long)CAN_IF1CRQ_MNUM_M)) | 0x01;

      //We have to wait 3-6 CAN CLOCK Wait then Busybit == 0 Ok
      while (1)
      {
          if (!(CAN1_IF1CRQ_R & CAN_IF1CRQ_BUSY))    //If not busy
              break;
          //busy->wait... may be 3-6 clock
      }

}




void printReceivedCanData(int uartNum, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4 )
{
    //Byte 0 is the first byte to be transmitted or received and data byte 7 is the last byte

   uint32_t a1_1=arg1;   //0x97
   uint32_t a1_2=(a1_1>>8)&0x000000ff;  //0x01

   uint32_t a2_1=arg2;   //0x00
   uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //0x2B

   uint32_t b1_1=arg3;   //0x00
   uint32_t b1_2=(uint32_t)((b1_1>>8)&0x000000ff);  //0x00

   uint32_t b2_1=arg4;   //0x00
   uint32_t b2_2=(uint32_t)((b2_1>>8)&0x000000ff);  //0x7D


   char DA1[5]=
   {    ((char)a1_1>>4),
        ((char)a1_1 & 0x0f),
        ((char)a1_2>>4),
        ((char)a1_2 & 0x0f),  };

   char DA2[5]=
   {    ((char)a2_1>>4),
        ((char)a2_1 & 0x0f),
        ((char)a2_2>>4),
        ((char)a2_2 & 0x0f),   };

   char DB1[5]=
   {    ((char)b1_1>>4),
        ((char)b1_1 & 0x0f),
        ((char)b1_2>>4),
        ((char)b1_2 & 0x0f),   };

   char DB2[5]=
   {    ((char)b2_1>>4),
        ((char)b2_1 & 0x0f),
        ((char)b2_2>>4),
        ((char)b2_2 & 0x0f),'\n'   };

   int i;


   for(i=0;i<4;i++)
    {
        if ( (DA1[i]>=0) && (DA1[i]<=9) ) DA1[i]=DA1[i]+'0';
        else if ( (DA1[i]>=10) && (DA1[i]<=15) ) DA1[i]=DA1[i]+'A'-10;
        else DA1[i]='z';
    }

   for(i=0;i<4;i++)
   {
       if ( (DA2[i]>=0) && (DA2[i]<=9) ) DA2[i]=DA2[i]+'0';
       else if ( (DA2[i]>=10) && (DA2[i]<=15) ) DA2[i]=DA2[i]+'A'-10;
       else DA2[i]='z';
   }


    for(i=0;i<4;i++)
    {
      if ( (DB1[i]>=0) && (DB1[i]<=9) ) DB1[i]=DB1[i]+'0';
      else if ( (DB1[i]>=10) && (DB1[i]<=15) ) DB1[i]=DB1[i]+'A'-10;
      else DB1[i]='z';
    }

    for(i=0;i<4;i++)
    {
       if ( (DB2[i]>=0) && (DB2[i]<=9) ) DB2[i]=DB2[i]+'0';
       else if ( (DB2[i]>=10) && (DB2[i]<=15) ) DB2[i]=DB2[i]+'A'-10;
       else DB2[i]='z';
    }

    if(uartNum==1)
    {
        uart_tx_str(DA1);
        uart_tx_str(DA2);
        uart_tx_str(DB1);
        uart_tx_str(DB2);
    }
    else if(uartNum==7)
    {
        uart_7_tx_str(DA1);
        uart_7_tx_str(DA2);
        uart_7_tx_str(DB1);
        uart_7_tx_str(DB2);
        //uart_7_tx_str("\r\n");
    }
}

void CanToSpi(char* (*a),unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4)
{

     uint32_t a1_1=arg1;   //0x97
     uint32_t a1_2=(a1_1>>8)&0x000000ff;  //0x01

     uint32_t a2_1=arg2;   //0x00
     uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //0x2B

     uint32_t b1_1=arg3;   //0x00
     uint32_t b1_2=(uint32_t)((b1_1>>8)&0x000000ff);  //0x00

     uint32_t b2_1=arg4;   //0x00
     uint32_t b2_2=(uint32_t)((b2_1>>8)&0x000000ff);  //0x7D

     static char DA1[17]={0,};

     DA1[0]=          ((char)a1_1>>4);
     DA1[1]=          ((char)a1_1 & 0x0f);
     DA1[2]=          ((char)a1_2>>4);
     DA1[3]=          ((char)a1_2 & 0x0f);
     DA1[4]=          ((char)a2_1>>4);
     DA1[5]=          ((char)a2_1 & 0x0f);
     DA1[6]=          ((char)a2_2>>4);
     DA1[7]=          ((char)a2_2 & 0x0f);
     DA1[8]=          ((char)b1_1>>4);
     DA1[9]=          ((char)b1_1 & 0x0f);
     DA1[10]=          ((char)b1_2>>4);
     DA1[11]=          ((char)b1_2 & 0x0f);
     DA1[12]=          ((char)b2_1>>4);
     DA1[13]=          ((char)b2_1 & 0x0f);
     DA1[14]=          ((char)b2_2>>4);
     DA1[15]=          ((char)b2_2 & 0x0f);
     DA1[16]=          '\0';



     int i;

     for(i=0;i<16;i++)
      {
          if ( (DA1[i]>=0) && (DA1[i]<=9) ) DA1[i]=DA1[i]+'0';
          else if ( (DA1[i]>=10) && (DA1[i]<=15) ) DA1[i]=DA1[i]+'A'-10;
          else DA1[i]='z';
      }

     *a=DA1;

}


void can_to_spi_rpm(char* (*a),unsigned int arg1)
{
    //can_rx_conf(can_id); //RPM

    uint32_t a2_1=arg1;   //CAN1_IF1DA2_R
    uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //

    static char DB1[2]={0,};

    DB1[0]=((char)a2_2>>4);
    DB1[1]=((char)a2_2 & 0x0f);

    DB1[0]=DB1[0]+'0';

    static char DB2[8]={'0','0','0','0','R','P','M','\0'};
    DB2[0]=DB1[0];

    *a=DB2;
}

void can_to_spi_acceleration(char* (*a),unsigned int arg1)
{
    //can_rx_conf(can_id); //Speed

    uint32_t a2_1=arg1;   //real car  CAN1_IF1DA2_R
   uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //0x2B -> debugging

    static char DD1[2]={0,};

//    //Real code
    DD1[0]=((char)a2_1>>4);
    DD1[1]=((char)a2_1 & 0x0f);


  // Debugging
    DD1[0]=((char)a2_2>>4);
    DD1[1]=((char)a2_2 & 0x0f);


    static char DD2[8]={0,0,0,'K','m','/','h','\0'};

    int i;

    i=DD1[0]*16+DD1[1];

    if((i/100)==0) DD2[0]=0x20;       else DD2[0]=i/100 + '0';
    if((i%100)/10 ==0) DD2[1]=0x20;   else DD2[1]=(i%100)/10 + '0';

    DD2[2]=((i%100)%10) + '0';

    *a=DD2;
}
void can_to_spi_transmission(char* (*a),unsigned int arg1)
{
//    can_rx_conf(arg1); //RPM

    uint32_t a1_1=arg1;   //CAN1_IF1DA1_R
    uint32_t a1_2=(a1_1>>8)&0x000000ff;  //0x01

    static char DC1[2]={0,};

    DC1[0]=((char)a1_2>>4);
    DC1[1]=((char)a1_2 & 0x0f);

    static char DC2[2]={'0','\0'};

    switch(DC1[1]) {
    case 5 :
        DC2[0]='D';
        break;
    case 6 :
        DC2[0]='N';
        break;
    case 7 :
        DC2[0]='R';
        break;
    case 0 :
        DC2[0]='P';
        break;
    default:
        DC2[0]='P';
    }

    *a=DC2;
}

void check_wake_state(unsigned int arg1)
{
    while(1)
    {
        //can_rx_conf(can_id); //transmission

        uint32_t a1_1=arg1;   //CAN1_IF1DA2_R
        uint32_t a1_2=(a1_1>>8)&0x000000ff;  //0x01

        static char DC1[2]={0,};

        DC1[0]=((char)a1_2>>4);
        DC1[1]=((char)a1_2 & 0x0f);

        if(DC1[1] == 0) continue;
        else  break;
    }
}

void can_to_spi_rpm2(int can_id, char* (*a))
{
    can_rx_conf(can_id); //RPM
    uint32_t a2_1=CAN1_IF1DA2_R;   //CAN1_IF1DA2_R
    //uint32_t a2_1=arg1;   //CAN1_IF1DA2_R
    uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //

    static char DB1[2]={0,};

    DB1[0]=((char)a2_2>>4);
    DB1[1]=((char)a2_2 & 0x0f);


    DB1[0]=DB1[0]+'0';

    static char DB2[8]={'0','0','0','0','R','P','M','\0'};
    DB2[0]=DB1[0];

    *a=DB2;

}

void can_to_spi_transmission2(int can_id, char* (*a))
{
    can_rx_conf(can_id); //RPM
    uint32_t a1_1=CAN1_IF1DA1_R;   //CAN1_IF1DA1_R
    //uint32_t a1_1=arg1;   //CAN1_IF1DA1_R
    uint32_t a1_2=(a1_1>>8)&0x000000ff;  //0x01

    static char DC1[2]={0,};

    DC1[0]=((char)a1_2>>4);
    DC1[1]=((char)a1_2 & 0x0f);

    static char DC2[2]={'0','\0'};

    switch(DC1[1]) {
    case 5 :
        DC2[0]='D';
        break;
    case 6 :
        DC2[0]='N';
        break;
    case 7 :
        DC2[0]='R';
        break;
    case 0 :
        DC2[0]='P';
        break;
    default:
        DC2[0]='P';
    }

    *a=DC2;

}

void can_to_spi_acceleration2(int can_id, char* (*a))
{
    can_rx_conf(can_id); //Speed

    uint32_t a2_1=CAN1_IF1DA1_R;   //Debugging CAN1_IF1DA1_R
    //uint32_t a2_1=arg1;   //real car  CAN1_IF1DA2_R
    uint32_t a2_2=(uint32_t)((a2_1>>8)&0x000000ff);  //0x2B

    static char DD1[2]={0,};

    DD1[0]=((char)a2_1>>4);
    DD1[1]=((char)a2_1 & 0x0f);


    static char DD2[8]={0,0,0,'K','m','/','h','\0'};

    int i;

    i=DD1[0]*16+DD1[1];

    if((i/100)==0) DD2[0]=0x20;       else DD2[0]=i/100 + '0';
    if((i%100)/10 ==0) DD2[1]=0x20;   else DD2[1]=(i%100)/10 + '0';

    DD2[2]=((i%100)%10) + '0';

    *a=DD2;

}


