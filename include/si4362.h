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
} SI4362_t;

#ifdef __cplusplus
extern "C"
{
#endif

  void si4362_init(SI4362_t *RF);
  void si4362_command(SI4362_t *RF, uint8_t CMD, uint8_t argsLength, uint8_t *args);
  bool si4362_read(SI4362_t *RF, uint8_t bufferLength, uint8_t *respBuffer);
  void si4362_reset(SI4362_t *RF);
  void si4362_wait_cts(SI4362_t *RF);
  void si4362_start_read(SI4362_t *RF);

  bool si4362_power_up(SI4362_t *RF, POWER_UP_ARGUMENTS *args);
  bool si4362_get_part_info(SI4362_t *RF, PART_INFO_RESPONSE *response);
  void si4362_get_device_state(SI4362_t *RF);
  void si4362_change_state(SI4362_t *RF, uint8_t state);
  void si4362_set_property(SI4362_t *RF, uint8_t group, uint8_t propSize, uint8_t startProp, uint8_t *properties);

#ifdef __cplusplus
}
#endif

#endif