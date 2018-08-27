#ifndef _TIMER_INTERRUPT_H_
 #define _TIMER_INTERRUPT_H_

 #ifdef __cplusplus
   extern "C" {
 #endif

  #define TRUE  ((boolean) 1)
  #define FALSE ((boolean) 0)

  extern void timer_init( void (*interrupt_func)(void));


//Definitions

  #ifdef __cplusplus
    }
  #endif

  #define milsec  0
  #define seconds 1
  #define minutes 2

#endif //_TIMER_INTERRUPT_H_




