//
// Created by Stanislav Lakhtin on 02/03/2017.
//
#include <libopencm3/stm32/f1/gpio.h>

#define ENCODER_PORT GPIOA
#define ENCODER_A GPIO8
#define ENCODER_B GPIO9

int encoder_setup(void) {
    gpio_set_mode(ENCODER_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, ENCODER_A | ENCODER_B);
}
