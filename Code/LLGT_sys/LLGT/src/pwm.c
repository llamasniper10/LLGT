// inc
//-------------------------------------------------------------------------
#include "pwm.h"


// proto
//-------------------------------------------------------------------------
/*
void PWM_Init (void);
void PWM_DC_set (uint_t pct);
*/

//-------------------------------------------------------------------------
void PWM_Init (void) {
//-------------------------------------------------------------------------
   CCP4CON1Lbits.CCSEL     = 0;        // set ccp operating mode (oc mode)
   CCP4CON1Lbits.MOD       = 5;        // set mode (buffered dual-compare/pwm mode)
                           
   CCP4CON1Lbits.T32       = 0;        // set timebase width (16-bit)
   CCP4CON1Lbits.TMRSYNC   = 0;        // synchronized
   CCP4CON1Lbits.CLKSEL    = 0;        // set clock source - peripheral
   CCP4CON1Lbits.TMRPS     = 0;        // set clock pre-scaler
   
   CCP4CON2Hbits.OCAEN     = 1;        // enable output signal (OC4A)
   CCP4CON3Hbits.POLACE    = 0;        // output polarity - active high
   CCP4TMRL                = 0;                 // clear timer
   CCP4PRL                 = SYS_PWM_PERIOD;    // set period
   CCP4RA                  = 0;                 // rising edge compare value
   CCP4RB                  = 1;                 // falling edge compare value (high time)
   
   CCP4CON1Lbits.CCPON     = 1;        // enable ccp module
}

//-------------------------------------------------------------------------
void PWM_DC_set (uint_t pct) {
//-------------------------------------------------------------------------
   uint_t   tmp;
   
   if (pct > 100)
      CCP4RB = SYS_PWM_PERIOD+1;
   else {
      tmp  = 100 * SYS_PWM_PERIOD;
      tmp /= pct;
      CCP4RB = tmp;                 // set high time
   }
}