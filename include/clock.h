#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <sam.h>
#include <stdint.h>

extern uint32_t SystemCoreClock;
extern volatile uint32_t sys_millis;

#define millis() sys_millis

#ifdef __cplusplus
extern "C"
{
#endif

  void initSystemClock();
  void delay(uint32_t time);

#ifdef __cplusplus
}
#endif

#endif