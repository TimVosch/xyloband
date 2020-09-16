#ifndef _BOARD_H_
#define _BOARD_H_

#include "sam.h"

#define LED_RED PORT_PA23
#define LED_GREEN PORT_PA22
#define LED_BLUE PORT_PA24
#define MOSFET PORT_PA10
#define SPEAKER_AIN PORT_PA02
#define SPEAKER_SHDN PORT_PA03

// PMUX D SERCOM 0
#define RF_SS PIN_PA05   // PAD 1
#define RF_SDI PIN_PA06  // PAD 2
#define RF_SDO PIN_PA04  // PAD 0
#define RF_SCLK PIN_PA07 // PAD 3
#define RF_SDN PIN_PA14
#define RF_GPIO1 PIN_PA00

#endif // !_BOARD_H_