#include <string.h>

#include "si4362.h"
#include "clock.h"
#include "sam.h"

void si4362_init(SI4362_t *RF)
{
  PORT->Group[0].DIRSET.reg = 1 << RF->SDN;
  PORT->Group[0].DIRCLR.reg = (1 << RF->GPIO1) | (1 << RF->SDN);
  PORT->Group[0].PINCFG[RF->GPIO1].bit.INEN = 1;

  // Initialize SPI communication
  spi_initialize(RF->SPI);

  // Soft reset module
  si4362_reset(RF);

  // Wait for boot
  si4362_wait_cts(RF);
}

/**
 * @brief Send a command with optional arguments to the SI4362 module
 * 
 * @param RF Module state
 * @param CMD The command byte
 * @param argsLength Length of the arguments buffer
 * @param args Pointer to the arguments buffer
 */
void si4362_command(SI4362_t *RF, uint8_t CMD, uint8_t argsLength, uint8_t *args)
{
  SPI_t *SPI = RF->SPI;

  spi_select(SPI);

  // Send command
  spi_transfer(SPI, CMD);

  // Send arguments if there are any
  if (argsLength > 0)
  {
    for (; argsLength; argsLength--)
    {
      uint8_t byte = *(args++);
      spi_transfer(SPI, byte);
    }
  }
  delay(1);
  spi_deselect(SPI);
}

/**
 * @brief Read a response stream from the SI4362 module
 * 
 * @param RF 
 * @param bufferLength The length of the response
 * @param respBuffer Pointer to the response buffer
 * @return true If the read was a succes
 * @return false If the module was not ready
 */
bool si4362_read(SI4362_t *RF, uint8_t bufferLength, uint8_t *respBuffer)
{
  SPI_t *SPI = RF->SPI;

  si4362_start_read(RF);

  // Radio is ready
  for (; bufferLength; bufferLength--)
  {
    *(respBuffer++) = spi_transfer(SPI, 0);
  }

  spi_deselect(SPI);

  return true;
}

/**
 * @brief Resets the SI4362 module with a 1ms delay
 * 
 * @param RF 
 */
void si4362_reset(SI4362_t *RF)
{
  PORT->Group[0].OUTSET.reg = 1 << (RF->SDN);
  delay(10);
  PORT->Group[0].OUTCLR.reg = 1 << (RF->SDN);
  delay(10);
}

/**
 * @brief Checks Clear To Send byte
 * 
 * @param RF 
 * @return true if CTS was signalled, spi kept active
 * @return false CTS not ready, spi closed
 */
void si4362_start_read(SI4362_t *RF)
{
  SPI_t *SPI = RF->SPI;

  uint8_t response = 0x00;

  while (response != 0xFF)
  {
    spi_select(SPI);
    delay(1);
    spi_transfer(SPI, READ_CMD_BUFF_CMD);
    response = spi_transfer(SPI, 0);

    if (response != 0xFF)
    {
      spi_deselect(SPI);
      delay(10);
    }
  }
}

/**
 * @brief Check module CTS status
 * 
 * @param RF 
 */
void si4362_wait_cts(SI4362_t *RF)
{
  si4362_start_read(RF);
  spi_deselect(RF->SPI);
}

/**
 * @brief 
 * 
 * @param RF 
 * @param args 
 * @return true 
 * @return false 
 */
bool si4362_power_up(SI4362_t *RF, POWER_UP_ARGUMENTS *args)
{
  si4362_wait_cts(RF);

  // Send CMD
  uint8_t buffer[6] = {
      0x1, 0x1, 0x01, 0xc9, 0xc3, 0x80};
  si4362_command(RF, POWER_UP_CMD, 6, buffer);

  // Expect CTS read to succeed
  si4362_wait_cts(RF);

  return true;
}

/**
 * @brief 
 * 
 * @param RF 
 * @param response 
 * @return true 
 * @return false 
 */
bool si4362_get_part_info(SI4362_t *RF, PART_INFO_RESPONSE *response)
{
  si4362_wait_cts(RF);

  si4362_command(RF, PART_INFO_CMD, 0, nullptr);
  uint8_t buf[8];
  si4362_read(RF, 8, buf);

  return buf[2] != 0xff;
}

void si4362_change_state(SI4362_t *RF, uint8_t state)
{
  si4362_wait_cts(RF);

  si4362_command(RF, CHANGE_STATE_CMD, 1, &state);

  si4362_wait_cts(RF);
}

void si4362_set_property(SI4362_t *RF, uint8_t group, uint8_t propSize, uint8_t startProp, uint8_t *properties)
{
  si4362_wait_cts(RF);

  uint8_t data[3 + propSize];
  data[0] = group;
  data[1] = propSize;
  data[2] = startProp;
  memcpy(&data[3], properties, propSize);
  si4362_command(RF, CHANGE_STATE_CMD, 1, data);

  si4362_wait_cts(RF);
}

void si4362_get_device_state(SI4362_t *RF)
{
  si4362_wait_cts(RF);

  si4362_command(RF, REQUEST_DEVICE_STATE_CMD, 0, nullptr);

  uint8_t response[2] = {0, 0};
  si4362_read(RF, 2, response);

  si4362_wait_cts(RF);
}