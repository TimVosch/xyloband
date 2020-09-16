#if !defined(__SI4362_COMMANDS_H__)
#define __SI4362_COMMANDS_H__

#include <stdint.h>

enum
{
  NOP_CMD = 0x00,
  PART_INFO_CMD = 0x01,
  POWER_UP_CMD = 0x02,
  FUNC_INFO_CMD = 0x10,
  SET_PROPERTY_CMD = 0x11,
  GET_PROPERTY_CMD = 0x12,
  GPIO_PIN_CFG_CMD = 0x13,
  GET_ADC_READING_CMD = 0x14,
  FIFO_INFO_CMD = 0x15,
  PACKET_INFO_CMD = 0x16,
  IRCAL_CMD = 0x17,
  IRCAL_MANUAL_CMD = 0x1a,
  GET_INT_STATUS_CMD = 0x20,
  GET_PH_STATUS_CMD = 0x21,
  GET_MODEM_STATUS_CMD = 0x22,
  GET_CHIP_STATUS_CMD = 0x23,
  START_RX_CMD = 0x32,
  REQUEST_DEVICE_STATE_CMD = 0x33,
  CHANGE_STATE_CMD = 0x34,
  RX_HOP_CMD = 0x36,
  READ_CMD_BUFF_CMD = 0x44,
  FRR_A_READ_CMD = 0x50,
  FRR_B_READ_CMD = 0x51,
  FRR_C_READ_CMD = 0x53,
  FRR_D_READ_CMD = 0x57,
  READ_RX_FIFO_CMD = 0x77
};

//
// Command arguments
//

typedef union
{
  struct
  {
    uint8_t FUNC : 7;
    uint8_t : 1;
    uint8_t PATCH : 1;
  } bit;
  uint8_t reg;
} BOOT_OPTIONS_t;

typedef union
{
  struct
  {
    uint8_t TCX0 : 1;
  } bit;
  uint8_t reg;
} XTAL_OPTIONS_t;

typedef struct
{
  BOOT_OPTIONS_t BOOT_OPTIONS;
  XTAL_OPTIONS_t XTAL_OPTIONS;
  uint32_t XO_FREQ;
} POWER_UP_ARGUMENTS;

//
// Command responses
//

typedef struct
{
  uint8_t CTS;
  uint8_t CHIPREV;
  uint8_t PARTh;
  uint8_t PARTl;
  uint8_t PBUILD;
  uint8_t IDh;
  uint8_t IDl;
  uint8_t CUSTOMER;
  uint8_t ROMID;
} PART_INFO_RESPONSE;

#endif