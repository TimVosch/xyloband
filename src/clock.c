#include "clock.h"

uint32_t SystemCoreClock = 1e6;

volatile uint32_t sys_millis = 0;

void SysTick_Handler()
{
  sys_millis++;
}

void delay(uint32_t time)
{
  uint32_t target = millis() + time;
  while (millis() < target)
    ;
}

void initSystemClock()
{
  SYSCTRL->OSC8M.bit.PRESC = 0;
  SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
  GCLK->GENDIV.reg = GCLK_GENDIV_ID_GCLK0;

  SystemCoreClock = 8e6;

  // every millisecond
  SysTick_Config(SystemCoreClock / 1000);
}