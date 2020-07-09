// inc
//-------------------------------------------------------------------------
#include "uart.h"


// var
//-------------------------------------------------------------------------
static FF_t  u1txFifo = {.hd = 0, .tl = 0};
static FF_t *pF = &u1txFifo;


// proto
//-------------------------------------------------------------------------
/*
void   UART_Init (void);
uint_t UART_Tx (uint_t len, u8 *pDa);
void   UART_Tx_Task (void);
*/

//-------------------------------------------------------------------------
void UART_Init (void) {
//-------------------------------------------------------------------------
   U1MODEbits.STSEL = 0;                  // 1-stop bit
   U1MODEbits.PDSEL = 0;                  // no parity, 8-data bits
   U1MODEbits.ABAUD = 0;                  // auto-baud disabled
   U1MODEbits.BRGH = 0;                   // standard speed mode
   if (U1MODEbits.BRGH)                   // baud rate setting
      U1BRG = SYS_UART_BRGH1_VAL;
   else
      U1BRG = SYS_UART_BRGH0_VAL;
   U1STAbits.UTXISEL0 = 0;                // interrupt when each tx character sent
   U1STAbits.UTXISEL1 = 0;
   U1MODEbits.UARTEN = 1;                 // enable uart
   U1STAbits.UTXEN = 1;                   // enable uart tx
   IFS0bits.U1TXIF = 0;                   // clear interrupt
   IEC0bits.U1TXIE = 1;                   // enable interrupt
}

//-------------------------------------------------------------------------
uint_t UART_Tx (uint_t len, u8 *pDa) {
//-------------------------------------------------------------------------
   uint_t   nxt;

   if (pDa == NULL) return len;        // validate source data pointer
   if (len == 0) return len;           // validate length isn't zero
   while (len) {
      nxt = (pF->hd+1) & (UART_TX_FIFO_DEPTH-1);
      if (nxt == pF->tl) return len;   // return remaining byte count
      pF->da[pF->hd] = *(pDa++);       // write data to fifo
      pF->hd = nxt;                    // advance head index
      len--;                           // upate remaining byte count
   }
   return 0;
}

//-------------------------------------------------------------------------
void UART_Tx_Task (void) {
//-------------------------------------------------------------------------
   FF_t *pF = &u1txFifo;

   // check hardware/software fifo
   while ((U1STAbits.UTXBF == 0) && (pF->tl != pF->hd)) {
      U1TXREG = (uint_t)(pF->da[pF->tl]);
      pF->tl = (pF->tl+1) & (UART_TX_FIFO_DEPTH-1);
   }
}
