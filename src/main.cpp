#include "main.h"
#include "spi.h"
#include "si4362.h"

SPI_t RF_SPI;
SI4362_t RF;
uint32_t STATUS = LED_RED;

void rf_init()
{
  STATUS = LED_RED;
  // Initialize SPI
  RF_SPI = spi_create(RF_SDI, RF_SDO, RF_SCLK, RF_SS);

  // Create SI4362 options
  RF = {
      .SPI = &RF_SPI,
      .SDN = RF_SDN,
      .GPIO1 = RF_GPIO1,
  };

  // Initializes ports
  si4362_init(&RF);

  si4362_change_state(&RF, 0x3);
  si4362_get_device_state(&RF);

  // Send power up command
  si4362_power_up(&RF, nullptr);

  si4362_command(&RF, PART_INFO_CMD, 0, nullptr);

  uint8_t data[8] = {0};

  uint8_t ready = si4362_read(&RF, 8, data);
  if (ready == 0)
  {
    return;
  }

  uint16_t part = (data[1] << 8) | data[2];
  if (part == 0x4362)
  {
    STATUS = LED_BLUE;
  }
}

int main(void)
{
  PORT->Group[0].DIRSET.reg = MOSFET |                        // Output to mosfet power switch for circuit
                              LED_GREEN | LED_RED | LED_BLUE; // LED Output

  PORT->Group[0].OUTCLR.reg = LED_GREEN | LED_RED | LED_BLUE | SPEAKER_SHDN;
  PORT->Group[0].OUTSET.reg = MOSFET | SPEAKER_SHDN;

  rf_init();

  while (1)
  {
    delay(1000);
    PORT->Group[0].OUTTGL.reg = STATUS;
  }
}