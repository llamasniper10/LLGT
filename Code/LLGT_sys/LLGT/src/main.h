#ifndef _MAIN_H_
#define _MAIN_H_


// inc
//-------------------------------------------------------------------------
#include <stddef.h>                     // Defines NULL
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>


// macro
//-------------------------------------------------------------------------
#define _ISR_     __attribute__((__interrupt__,no_auto_psv)) 


// param
//-------------------------------------------------------------------------
#define SYS_CLK_FREQ             8000000

#define SYS_TMR_PERIOD           (unsigned int)(SYS_CLK_FREQ / 16)

#define SYS_UART_BAUDRATE        9600
#define SYS_UART_BRGH0_VAL       (unsigned int)((SYS_CLK_FREQ / SYS_UART_BAUDRATE / 16)-1)
#define SYS_UART_BRGH1_VAL       (unsigned int)((SYS_CLK_FREQ / SYS_UART_BAUDRATE / 4)-1)

#define SYS_PWM_FREQ             50000
#define SYS_PWM_PERIOD           (SYS_CLK_FREQ / SYS_PWM_FREQ)


// type
//-------------------------------------------------------------------------
typedef uint8_t      u8       ;
typedef int8_t       s8       ;
typedef uint16_t     u16      ;
typedef int16_t      s16      ;
typedef uint32_t     u32      ;
typedef int32_t      s32      ;
typedef uint16_t     uint_t   ;     // native unsigned type
typedef int16_t      int_t    ;     // native signed type

#endif