//
// Created by sl on 18.02.17.
//

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <pcd8544.h>
#include <stdio.h>
#include <errno.h>

static void clock_setup(void) {
  rcc_clock_setup_in_hse_8mhz_out_72mhz();

  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);

  /* Enable SPI2 Periph and gpio clocks */
  rcc_periph_clock_enable(RCC_SPI2);

  rcc_periph_clock_enable(RCC_USART2);
  rcc_periph_clock_enable(RCC_USART3);
}

static void gpio_setup(void) {
  /* Configure GPIOs */
  gpio_set_mode(MOTOR_PORT, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                MOTOR_A0 | MOTOR_A1 | MOTOR_B0 | MOTOR_B1);
  gpio_clear(MOTOR_PORT, MOTOR_A0 | MOTOR_A1 | MOTOR_B0 | MOTOR_B1);
}

int _write(int file, char *ptr, int len) {
  int i;

  if (file == 1) {
    for (i = 0; i < len; i++)
      usart_send_blocking(USART2, ptr[i]);
    return i;
  }
  errno = EIO;
  return -1;
}

static void usart_setup(void) {
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                GPIO_CNF_INPUT_PULL_UPDOWN, GPIO_USART2_RX);

  usart_set_baudrate(USART2, 9600);
  usart_set_databits(USART2, 8);
  usart_set_stopbits(USART2, USART_STOPBITS_1);
  usart_set_mode(USART2, USART_MODE_TX_RX);
  usart_set_parity(USART2, USART_PARITY_NONE);
  usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
  usart_enable(USART2);
}


int main(void) {
  clock_setup();
  gpio_setup();
  usart_setup();

  while (1) {
  }
}

