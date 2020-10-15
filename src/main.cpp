#include "main.h"
#include "spi.h"
#include "si4362.h"

SPI_t RF_SPI;
SI4362_t RF;
uint32_t STATUS = LED_BLUE;

/**
 * 
 */
void set_status(uint32_t pin, uint8_t freq)
{
  uint16_t ms = (1.0f / freq * 1000);
  while (1)
  {
    PORT->Group[0].OUTTGL.reg = pin;
    delay(ms);
  }
}

void assertOk(RF_Status s, uint8_t speed)
{
  if (s == RF_NOT_READY)
  {
    set_status(LED_BLUE, speed);
  }
}

/**
 * 
 */
RF_Status rf_init()
{
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

  // Send power up command
  si4362_power_up(&RF, nullptr);

  PART_INFO_RESPONSE info = {0};
  si4362_get_part_info(&RF, &info);

  uint16_t part = (info.PARTh << 8) | info.PARTl;
  if (part == 0x4362)
  {
    return RF_READY;
  }
  return RF_NOT_READY;
}

/**
 * 
 */
RF_Status rf_init_rx()
{
  RF_Status s;

  // Set CLK division for PLL
  s = si4362_modem_clkgen_band(&RF, 1, 0x2);
  assertOk(s, 4);

  // Set frequency for PLL
  s = si4362_freq_control(&RF, 56, 909000);
  assertOk(s, 5);

  // Set OOK
  s = si4362_modem_mod_type(&RF, 0x1);
  assertOk(s, 6);

  s = si4362_set_gpio0_rx_state(&RF);
  assertOk(s, 7);

  // Start RX
  s = si4362_start_rx(&RF, 1);
  assertOk(s, 8);

  return RF_READY;
}

int main(void)
{
  PORT->Group[0].DIRSET.reg = MOSFET |                        // Output to mosfet power switch for circuit
                              LED_GREEN | LED_RED | LED_BLUE; // LED Output

  PORT->Group[0].OUTCLR.reg = LED_GREEN | LED_RED | LED_BLUE | SPEAKER_SHDN;
  PORT->Group[0].OUTSET.reg = MOSFET | SPEAKER_SHDN;

  // Initialize RF module
  if (rf_init() != RF_READY)
  {
    set_status(LED_RED, 1);
  }

  // Configure RF_module for RX
  if (rf_init_rx() != RF_READY)
  {
    set_status(LED_RED, 2);
  }

  // Check if we are in RX
  uint8_t resp[3] = {0};
  si4362_get_device_state(&RF, &resp[0]);

  if (resp[1] != 0x8)
  {
    set_status(LED_BLUE, 5);
  }

  // Stay blue until we leave RX state
  PORT->Group[0].OUTSET.reg = LED_BLUE;
  uint8_t inRX = 1;
  while ((inRX = (PORT->Group[0].IN.reg & (1 << RF.GPIO1))) == 1)
  {
    delay(1000);
  }
  PORT->Group[0].OUTCLR.reg = LED_BLUE;

  // Only reached when no errors were thrown
  uint16_t ms = 1000;
  for (;;)
  {
    PORT->Group[0].OUTTGL.reg = LED_GREEN;
    delay(ms);
  }
}