/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "ch.h"
#include "hal.h"
#include "spi.h"
#include "board.h"
#include "stm32f0xx.h"
#include "pal.h"
#include "pwm.h"

/**
 * @brief   Initialize the board for the display.
 * @notes	This board definition uses GPIO and assumes exclusive access to these GPIO pins
 * @notapi
 */
 
#define SET_A0()	palSetPad(GPIOA, 6);
#define CLR_A0()	palClearPad(GPIOA, 6);
#define SET_RST()	palSetPad(GPIOA, 3);
#define CLR_RST()	palClearPad(GPIOA, 3);
#define SET_CS1()	palSetPad(GPIOA, 4);
#define CLR_CS1()	palClearPad(GPIOA, 4);

static const SPIConfig spic = {
  NULL,
  GPIOA,
  GPIOA_PIN7,
  SPI_CR1_SPE | SPI_CR1_CPOL | SPI_CR1_BR_2, // 12MHz Baud rate?
  SPI_CR2_DS // 16-bit mode
};

static PWMConfig pwmcfg = {
  24000000, /* 24MHz, Max Timer Frequency */
  1000,     /* 0.004 ms period */
  NULL,     /* No callback */
  NULL,
  0
};

static inline void init_board(void) {
  /* Configure SPI bus here. Up to 10Mhz, SPI Mode = 3 (CPOL = 1, CPHA = 0) */
  spiStart(&SPID1, &spic);
  
  /* Configure A0, !CS, !RST pin for output */
  palSetGroupMode(GPIOA, PAL_WHOLE_PORT, 0, PAL_MODE_OUTPUT_PUSHPULL);
  
  /* Configure PWM Channels */
  PWMChannelConfig chcfg = { PWM_OUTPUT_ACTIVE_HIGH, NULL };
  pwmcfg.channels[0] = chcfg;
  
  /* Configure PWM Pin */
  palSetPadMode(GPIOB, 0, PAL_STM32_OTYPE_PUSHPULL);
  
  /* Start the PWM Thingy */
  pwmStart(&PWMD2, &pwmcfg);
}

/**
 * @brief   Set or clear the lcd reset pin.
 * @param[in] state		TRUE = lcd in reset, FALSE = normal operation
 * @notapi
 */
static inline void setpin_reset(bool_t state) {
  /* Set !CS to low */
  CLR_CS1();
  if (state) {
    /* Set !RST to low */
    CLR_RST();
  } else {
    /* Set !RST to high */
    SET_RST();
  }
}

/**
 * @brief   Set the lcd back-light level.
 * @param[in] percent		0 to 100%
 * @notapi
 */
static inline void set_backlight(uint8_t percent) {
  if (percent > 100)
    percent = 100;
  
  pwmEnableChannel(&PWMD2, 0, (pwmcnt_t)(percent * 655));
}

/**
 * @brief   Take exclusive control of the busdata
 * @notapi
 */
static inline void acquire_bus(void) {
  spiAcquireBus(&SPID1);
}

/**
 * @brief   Release exclusive control of the bus
 * @notapi
 */
static inline void release_bus(void) {
  spiReleaseBus(&SPID1);
}

/**
 * @brief   Send command to the display.
 * @param[in] cmd	The command to send
 * @notapi
 */
static inline void write_cmd(uint8_t cmd) {
  /* Set A0 to low */
  CLR_A0();
  /* Transmit cmd over SPI */
  spiSend(&SPID1, 1, &cmd);
}

/**
 * @brief   Send data to the display.
 * @param[in] data	The data to send
 * @param[in] length The length of the data being sent
 * @notapi
 */
static inline void write_data(uint8_t* data, uint16_t length) {
  /* Set A0 to high */
  SET_A0();
  /* Transmit data for length over SPI. DMA is recommended. */
  spiSend(&SPID1, length, data);
}

#endif /* _GDISP_LLD_BOARD_H */
/** @} */

