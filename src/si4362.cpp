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
RF_Status si4362_command(SI4362_t *RF, uint8_t CMD, uint8_t argsLength, uint8_t *args)
{
  SPI_t *SPI = RF->SPI;

  // Read CTS a few times and see if chip becomes ready
  if (si4362_wait_cts(RF) != RF_READY)
  {
    return RF_NOT_READY;
  }

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
  spi_deselect(SPI);

  return RF_READY;
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
RF_Status si4362_read(SI4362_t *RF, uint8_t bufferLength, uint8_t *respBuffer)
{
  SPI_t *SPI = RF->SPI;

  // Read CTS a few times and see if chip becomes ready
  if (si4362_wait_cts(RF) == RF_NOT_READY)
  {
    return RF_NOT_READY;
  }

  spi_select(SPI);

  // Begin reading response
  spi_transfer(SPI, READ_CMD_BUFF_CMD);

  // Radio is ready
  for (; bufferLength; bufferLength--)
  {
    *(respBuffer++) = spi_transfer(SPI, 0);
  }

  spi_deselect(SPI);

  return RF_READY;
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
 * @brief Checks Clear To Send line
 * 
 * @param RF 
 * @return radio status. Either RF_READY or RF_NOT_READY
 */
RF_Status si4362_ready(SI4362_t *RF)
{
  SPI_t *SPI = RF->SPI;

  uint8_t response = 0x00;

  while (response != 0xFF)
  {
    spi_select(SPI);
    spi_transfer(SPI, READ_CMD_BUFF_CMD);
    response = spi_transfer(SPI, 0x00);
    spi_deselect(SPI);

    if (response != 0xFF)
    {
      delay(10);
    }
  }

  return response == 0xff ? RF_READY : RF_NOT_READY;

  // Line is high when ready, low if busy
  // uint8_t ready = (PORT->Group[0].IN.reg & (1 << RF->GPIO1));

  // return ready > 0 ? RF_READY : RF_NOT_READY;
}

/**
 * @brief Check module CTS status
 * 
 * @param RF 
 */
RF_Status si4362_wait_cts(SI4362_t *RF)
{
  uint8_t retries = 5;
  while (si4362_ready(RF) != RF_READY && retries--)
  {
    delay(10);
  }
  return si4362_ready(RF);
}

/**
 * @brief 
 * 
 * @param RF 
 * @param args 
 * @return true 
 * @return false 
 */
RF_Status si4362_power_up(SI4362_t *RF, POWER_UP_ARGUMENTS *args)
{
  // Send CMD
  uint8_t buffer[6] = {
      0x1, 0x1, 0x01, 0xc9, 0xc3, 0x80};
  RF_Status status = si4362_command(RF, POWER_UP_CMD, 6, buffer);

  return status;
}

/**
 * @brief 
 * 
 * @param RF 
 * @param response 
 * @return true 
 * @return false 
 */
RF_Status si4362_get_part_info(SI4362_t *RF, PART_INFO_RESPONSE *response)
{
  RF_Status status = si4362_command(RF, PART_INFO_CMD, 0, nullptr);
  if (status == RF_NOT_READY)
  {
    return RF_NOT_READY;
  }

  status = si4362_read(RF, 9, (uint8_t *)response);
  if (status == RF_NOT_READY)
  {
    return RF_NOT_READY;
  }

  return RF_READY;
}

RF_Status si4362_change_state(SI4362_t *RF, uint8_t state)
{
  RF_Status status = si4362_command(RF, CHANGE_STATE_CMD, 1, &state);

  if (status == RF_NOT_READY)
  {
    return RF_NOT_READY;
  }

  return RF_READY;
}

RF_Status si4362_set_property(SI4362_t *RF, uint8_t group, uint8_t num_props, uint8_t start_prop, uint8_t *properties)
{
  uint8_t data[3 + num_props];
  data[0] = group;
  data[1] = num_props;
  data[2] = start_prop;
  memcpy(&data[3], properties, num_props);

  RF_Status status = si4362_command(RF, CHANGE_STATE_CMD, 1, data);

  if (status == RF_NOT_READY)
  {
    return RF_NOT_READY;
  }

  return RF_READY;
}

RF_Status si4362_get_device_state(SI4362_t *RF)
{
  RF_Status status = si4362_command(RF, REQUEST_DEVICE_STATE_CMD, 0, nullptr);

  if (status == RF_NOT_READY)
  {
    return RF_NOT_READY;
  }

  uint8_t response[2] = {0, 0};
  si4362_read(RF, 2, response);

  return RF_READY;
}

RF_Status si4362_modem_mod_type(SI4362_t *RF, uint8_t type)
{
  RF_Status status = si4362_set_property(RF, 0x20, 1, 0, &type);

  if (status == RF_NOT_READY)
  {
    return RF_NOT_READY;
  }

  return RF_READY;
}

RF_Status si4362_base_frequency(SI4362_t *RF, uint32_t freq)
{
  RF_Status status = si4362_set_property(RF, 0x20, 1, 0, &type);

  if (status == RF_NOT_READY)
  {
    return RF_NOT_READY;
  }

  return RF_READY;
}