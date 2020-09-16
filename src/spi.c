#include "sam.h"
#include "clock.h"
#include "spi.h"

SPI_t spi_create(uint16_t SDI, uint16_t SDO, uint16_t SCLK, uint16_t SS)
{
  return (SPI_t){
      .SDI = SDI,
      .SDO = SDO,
      .SCLK = SCLK,
      .SS = SS,
      .sercom = SERCOM0};
}

void spi_syncbusy(Sercom *sercom)
{
  // Wait for sync
  while (sercom->SPI.STATUS.bit.SYNCBUSY)
    ;
}

void spi_select(SPI_t *SPI)
{
  PORT->Group[0].OUTCLR.reg = SPI->SS;
  delay(1);
}

void spi_deselect(SPI_t *SPI)
{
  delay(1);
  PORT->Group[0].OUTSET.reg = SPI->SS;
}

void spi_initialize(SPI_t *SPI)
{
  Sercom *sercom = SPI->sercom;

  // Initialize PINS
  PORT->Group[0].DIRSET.reg = (1 << SPI->SCLK) | (1 << SPI->SDI) | (1 << SPI->SS);
  PORT->Group[0].OUTSET.reg = (1 << SPI->SS);
  PORT->Group[0].DIRCLR.reg = (1 << SPI->SDO);

  PORT->Group[0].PINCFG[SPI->SDI].reg = PORT_PINCFG_PMUXEN;
  PORT->Group[0].PINCFG[SPI->SDO].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN;
  PORT->Group[0].PINCFG[SPI->SCLK].reg = PORT_PINCFG_PMUXEN;
  PORT->Group[0].PINCFG[SPI->SS].reg = PORT_PINCFG_PULLEN;

  // Configure PMUX to peripheral D
  PORT->Group[0].PMUX[SPI->SDI / 2].reg |= 0x3 << ((SPI->SDI & 0x1) * 4);
  PORT->Group[0].PMUX[SPI->SDO / 2].reg |= 0x3 << ((SPI->SDO & 0x1) * 4);
  PORT->Group[0].PMUX[SPI->SCLK / 2].reg |= 0x3 << ((SPI->SCLK & 0x1) * 4);

  // Disable and reset SPI
  spi_syncbusy(sercom);
  sercom->SPI.CTRLA.bit.ENABLE = 0;
  sercom->SPI.CTRLA.bit.SWRST = 1;
  spi_syncbusy(sercom);

  // Enable SERCOM0 CLK
  PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

  // Configure clock
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
                      GCLK_CLKCTRL_GEN(0) | GCLK_CLKCTRL_CLKEN;

  SERCOM_SPI_CTRLA_Type spi_conf_a = {
      .bit = {
          .MODE = SERCOM_SPI_CTRLA_MODE_SPI_MASTER_Val, // Set mode to master
          .CPOL = 0x0,                                  // Set SCLK low on idle
          .CPHA = 0x0,                                  // Set sample on rising edge
          .DIPO = 0x0,                                  // Set PAD0 as input
          .DOPO = 0x1,                                  // Set PAD2 as ouput, PAD1 as SS and PAD 3 as SCLK
          .DORD = 0x0,                                  // Set Data order to MSB
          .RUNSTDBY = 0x01,                             // Run in STDBY mode
      }};
  SERCOM_SPI_CTRLB_Type spi_conf_b = {
      .bit = {
          .CHSIZE = 0x0, // Set Character size to 8 bits
          .RXEN = 0x1,   // Set RX enabled
      }};

  sercom->SPI.CTRLA.reg = spi_conf_a.reg;
  sercom->SPI.CTRLB.reg = spi_conf_b.reg;

  // Calculate and set baud rate
  uint16_t freq = 50000;
  uint16_t baud = ((float)SystemCoreClock / (2 * (float)freq)) - 1;
  sercom->SPI.BAUD.reg = SERCOM_SPI_BAUD_BAUD(baud);

  // Enable SPI
  spi_syncbusy(sercom);
  sercom->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
  spi_syncbusy(sercom);
}

uint8_t spi_transfer(SPI_t *SPI, uint8_t data)
{
  Sercom *sercom = SPI->sercom;

  while (sercom->SPI.INTFLAG.bit.DRE == 0)
    ;

  sercom->SPI.DATA.reg = data;

  while (sercom->SPI.INTFLAG.bit.RXC == 0 || sercom->SPI.INTFLAG.bit.TXC == 0)
    ;
  // Reset flags
  sercom->SPI.INTFLAG.reg = sercom->SPI.INTFLAG.reg;

  return (uint8_t)sercom->SPI.DATA.reg;
}