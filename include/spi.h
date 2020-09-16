#if !defined(__SPI_H__)
#define __SPI_H__

#include <stdint.h>
#include "sam.h"

typedef struct
{
  uint16_t SDI;
  uint16_t SDO;
  uint16_t SCLK;
  uint16_t SS;
  Sercom *sercom;
} SPI_t;

#ifdef __cplusplus
extern "C"
{
#endif

  SPI_t spi_create(uint16_t SDI, uint16_t SDO, uint16_t SCLK, uint16_t SS);
  void spi_initialize(SPI_t *SPI);
  uint8_t spi_transfer(SPI_t *SPI, uint8_t data);
  void spi_select(SPI_t *SPI);
  void spi_deselect(SPI_t *SPI);

#ifdef __cplusplus
}
#endif

#endif