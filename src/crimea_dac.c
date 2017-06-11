//
// Created by sl on 11.06.17.
//

#include <crimea_dac.h>
#include <libopencm3/stm32/gpio.h>

void pullUp_ON(void) {
  gpio_clear(USB_PULLUP_PORT, USB_PULLUP_GPIO);
}

void pullUp_OFF(void) {
  gpio_set(USB_PULLUP_PORT, USB_PULLUP_GPIO);
}
