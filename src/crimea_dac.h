//
// Created by sl on 11.06.17.
//

#ifndef CRIMEA_OPENDAC_CRIMEA_DAC_H
#define CRIMEA_OPENDAC_CRIMEA_DAC_H

#include <libopencm3/stm32/gpio.h>

#define USB_PULLUP_PORT GPIOB
#define USB_PULLUP_GPIO  GPIO6

#define PCM2707A_CS_PORT GPIOB
#define PCM2707A_CS_GPIO GPIO4

#define PCM2707A_SUSPEND_PORT GPIOB
#define PCM2702A_SUSPEND_GPIO GPIO0

#define PCM707A_WORD 0x08

// 2 bytes
#define USB_VENDOR_ID 0x08bb
// 2 bytes
#define USB_PRODUCT_ID 2704
// 16 bytes
#define USB_PRODUCT_NAME "Crimea USB DAC v1"
// 32 bytes
#define USB_VENDOR_STRING  "Stanslav Lakhtin DIY project    "
// 1 byte (0x80 -- bus powered)
#define USB_POWER_ATTRIBUTE 0x80
// 1 byte
#define USB_MAX_POWER 0x7d
//3 bytes
#define USB_AUX_HID_DEVICE_ID_1 0x0a
#define USB_AUX_HID_DEVICE_ID_2 0x93
#define USB_AUX_HID_DEVICE_ID_3 0x01


void pcm2707a_busReset(void);
void pullUp_ON(void);
void pullUp_OFF(void);

void pcm2707a_setup(void);

#endif //CRIMEA_OPENDAC_CRIMEA_DAC_H
