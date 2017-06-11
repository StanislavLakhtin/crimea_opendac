/*
 * Copyright (c) 2015, Tido Klaassen. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. No personal names or organizations' names associated with the
 *    Atomthreads project may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ATOMTHREADS PROJECT AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/spi.h>
#include <crimea_dac.h>
#include <atomport.h>
#include <libopencm3/stm32/i2c.h>
#include <ssd1306_i2c.h>

static void clock_setup(void) {
  rcc_clock_setup_in_hse_8mhz_out_72mhz();

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);

  /* set clock for I2C */
  rcc_periph_clock_enable(RCC_I2C2);

  /* Enable SPI1 Periph and gpio clocks */
  rcc_periph_clock_enable(RCC_SPI1);

  rcc_periph_clock_enable(RCC_AFIO);
  AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_JNTRST;
}

/**
 * Set up USART2.
 * This one is connected via the virtual serial port on the Nucleo Board
 */
/*static void usart2_setup(uint32_t baud) {

  usart_disable(USART2);

  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                GPIO_USART2_TX | GPIO_USART2_RX);

  usart_set_baudrate(USART2, baud);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
  usart_set_mode(USART2, USART_MODE_TX_RX);

  usart_enable(USART2);
}*/

static void spi1_setup(void) {

  /* Configure GPIOs: SS=PA4, SCK=PA5, MISO=PA6 and MOSI=PA7 */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO4 |
                                                GPIO5 |
                                                GPIO7 );

  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                GPIO6);

  /* Reset SPI, SPI_CR1 register cleared, SPI is disabled */
  spi_reset(SPI1);

  /* Set up SPI in Master mode with:
   * Clock baud rate: 1/64 of peripheral clock frequency
   * Clock polarity: Idle High
   * Clock phase: Data valid on 2nd clock pulse
   * Data frame format: 8-bit
   * Frame format: MSB First
   */
  spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

  /*
   * Set NSS management to software.
   *
   * Note:
   * Setting nss high is very important, even if we are controlling the GPIO
   * ourselves this bit needs to be at least set to 1, otherwise the spi
   * peripheral will not send any data out.
   */
  spi_enable_software_slave_management(SPI1);
  spi_set_nss_high(SPI1);

  /* Enable SPI1 periph. */
  spi_enable(SPI1);
}

/**
 * initialise and start SysTick counter. This will trigger the
 * sys_tick_handler() periodically once interrupts have been enabled
 * by archFirstThreadRestore()
 */
static void systick_setup(void) {
  systick_set_frequency(SYSTEM_TICKS_PER_SEC, 72000000);
  systick_interrupt_enable();
  systick_counter_enable();
}

void gpio_setup(void) {
  gpio_set_mode(USB_PULLUP_PORT, GPIO_MODE_OUTPUT_50_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, USB_PULLUP_GPIO);
  pullUp_OFF();

  gpio_set_mode(PCM2707A_CS_PORT, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL, PCM2707A_CS_GPIO);

  gpio_set_mode(PCM2707A_SUSPEND_PORT, GPIO_MODE_INPUT,
                GPIO_CNF_INPUT_FLOAT, PCM2702A_SUSPEND_GPIO);

}

static void i2c_setup(void) {
  /* Set alternate functions for the SCL and SDA pins of I2C2. */
  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                GPIO_I2C2_SCL | GPIO_I2C2_SDA);

  /* Disable the I2C before changing any configuration. */
  i2c_peripheral_disable(I2C2);

  /* APB1 is running at 36MHz. */
  i2c_set_clock_frequency(I2C2, I2C_CR2_FREQ_36MHZ);

  /* 400KHz - I2C Fast Mode */
  i2c_set_fast_mode(I2C2);

  /*
	 * fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
	 * incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
	 * Datasheet suggests 0x1e.
	 */
  i2c_set_ccr(I2C2, 0x1e);

  /*
   * fclock for I2C is 36MHz -> cycle time 28ns, rise time for
   * 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
   * Incremented by 1 -> 11.
   */
  i2c_set_trise(I2C2, 0x0b);

  /*
   * Enable ACK on I2C
   */
  i2c_enable_ack(I2C2);

  /*
   * This is our slave address - needed only if we want to receive from
   * other masters.
   */
  i2c_set_own_7bit_slave_address(I2C2, 0x32);

  /* If everything is configured -> enable the peripheral. */
  i2c_peripheral_enable(I2C2);
}

void display_WakeUp(void) {
  ssd1306_init(I2C2, DEFAULT_7bit_OLED_SLAVE_ADDRESS, 128, 32);
  ssd1306_refresh();
  ssd1306_clear();
  ssd1306_refresh();
}

/**
 * Callback from your main program to set up the board's hardware before
 * the kernel is started.
 */
int board_setup(void) {
  /* Disable interrupts. This makes sure that the sys_tick_handler will
   * not be called before the first thread has been started.
   * Interrupts will be enabled by archFirstThreadRestore().
   */
  cm_mask_interrupts(true);

  /* configure system clock, user LED and UART */
  clock_setup();
  gpio_setup();
  spi1_setup();
  pcm2707a_busReset();
  pcm2707a_setup();
  i2c_setup();
  display_WakeUp();

  /* initialise SysTick counter */
  systick_setup();

  /* Set exception priority levels. Make PendSv the lowest priority and
   * SysTick the second to lowest
   */
  nvic_set_priority(NVIC_PENDSV_IRQ, 0xFF);
  nvic_set_priority(NVIC_SYSTICK_IRQ, 0xFE);

  return 0;
}
