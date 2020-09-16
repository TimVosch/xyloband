#if !defined(__SI4362_H__)
#define __SI4362_H__

#include <stdint.h>
#include "spi.h"

#include "si4362_commands.h"

typedef struct
{
  SPI_t *SPI;
  uint8_t SDN;
  uint8_t GPIO1;
  uint8_t SY_SEL;
  uint8_t PLL_DIV;
} SI4362_t;

typedef enum
{
  RF_READY = 0,
  RF_NOT_READY,
} RF_Status;

#ifdef __cplusplus
extern "C"
{
#endif

  void si4362_init(SI4362_t *RF);
  RF_Status si4362_command(SI4362_t *RF, uint8_t CMD, uint8_t argsLength, uint8_t *args);
  RF_Status si4362_read(SI4362_t *RF, uint8_t bufferLength, uint8_t *respBuffer);
  void si4362_reset(SI4362_t *RF);
  RF_Status si4362_wait_cts(SI4362_t *RF);
  RF_Status si4362_ready(SI4362_t *RF);

  RF_Status si4362_power_up(SI4362_t *RF, POWER_UP_ARGUMENTS *args);
  RF_Status si4362_get_part_info(SI4362_t *RF, PART_INFO_RESPONSE *response);
  RF_Status si4362_get_device_state(SI4362_t *RF);
  RF_Status si4362_change_state(SI4362_t *RF, uint8_t state);
  RF_Status si4362_set_property(SI4362_t *RF, uint8_t group, uint8_t propSize, uint8_t startProp, uint8_t *properties);
  RF_Status si4362_modem_mod_type(SI4362_t *RF, uint8_t type);

#ifdef __cplusplus
}
#endif

#endif