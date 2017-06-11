//
// Created by sl on 11.06.17.
//

#ifndef CRIMEA_OPENDAC_CRIMEA_DAC_H
#define CRIMEA_OPENDAC_CRIMEA_DAC_H

#include <libopencm3/stm32/gpio.h>

#define USB_PULLUP_PORT GPIOB
#define USB_PULLUP_GPIO  GPIO6

void pullUp_ON(void);
void pullUp_OFF(void);

#endif //CRIMEA_OPENDAC_CRIMEA_DAC_H
