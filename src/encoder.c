//
// Created by Stanislav Lakhtin on 02/03/2017.
//
#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/f1/memorymap.h>
#include <libopencm3/stm32/f1/gpio.h>

#define ENCODER_PORT GPIOA
#define ENCODER_A GPIO8
#define ENCODER_B GPIO9
#define ENCODER_BUTTON GPIO10

uint32_t *vControl;

void encoder_setup(uint32_t *volumeControl) {
  // Clocks and GPIOs should be initialized in board_setup.c
  vControl = volumeControl;
}

void encoder_handler(void) {
  if (gpio_get(ENCODER_PORT, ENCODER_A) && !gpio_get(ENCODER_PORT, ENCODER_B))
    *vControl += 10;
  else if (gpio_get(ENCODER_PORT, ENCODER_A) && gpio_get(ENCODER_PORT, ENCODER_B))
    *vControl -= 10;
}
