#ifndef _UART_H_
#define _UART_H_


// inc
//-------------------------------------------------------------------------
#include "main.h"


// param
//-------------------------------------------------------------------------
#define UART_TX_FIFO_DEPTH          (1<<7)


// type
//-------------------------------------------------------------------------
typedef struct {
   uint_t   hd;                           // head
   uint_t   tl;                           // tail
   u8       da    [UART_TX_FIFO_DEPTH];   // data buffer
} FF_t;


// extern
//-------------------------------------------------------------------------
extern void   UART_Init (void);
extern uint_t UART_Tx (uint_t len, u8 *pDa);
extern void   UART_Tx_Task (void);

#endif