//
// Created by sl on 11.06.17.
//

#include <crimea_dac.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

void pullUp_ON(void) {
  gpio_clear(USB_PULLUP_PORT, USB_PULLUP_GPIO);
}

void pullUp_OFF(void) {
  gpio_set(USB_PULLUP_PORT, USB_PULLUP_GPIO);
}

uint8_t reverse(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

void pcm2707a_spi_send(uint8_t prefix, uint8_t data) {
  spi_xfer(SPI1, prefix);
  spi_xfer(SPI1, reverse(data));
}

void pcm2707a_busReset(void) {
  pullUp_OFF();
  int loop;
  for (loop = 0; loop < 1000000; ++loop) {
    __asm__("nop");
  }
  pullUp_ON();
}

void pcm2707a_setup(void) {
  gpio_clear(PCM2707A_CS_PORT, PCM2707A_CS_GPIO);
  pcm2707a_spi_send (0x02,0x00);

  pcm2707a_spi_send (PCM707A_WORD,0x00ff & USB_VENDOR_ID);
  pcm2707a_spi_send (PCM707A_WORD,USB_VENDOR_ID >> 8);

  pcm2707a_spi_send (PCM707A_WORD,0x00ff & USB_PRODUCT_ID);
  pcm2707a_spi_send (PCM707A_WORD,USB_PRODUCT_ID >> 8);

  int i = 0;
  for (; i < 16; ++i) {
    pcm2707a_spi_send(PCM707A_WORD, USB_PRODUCT_NAME[i]);
  }
  i = 0;
  for (; i < 32; ++i) {
    pcm2707a_spi_send(PCM707A_WORD, USB_VENDOR_STRING[i]);
  }
  pcm2707a_spi_send (PCM707A_WORD, USB_POWER_ATTRIBUTE);
  pcm2707a_spi_send (PCM707A_WORD, USB_MAX_POWER);
  pcm2707a_spi_send (PCM707A_WORD, USB_AUX_HID_DEVICE_ID_1);
  pcm2707a_spi_send (PCM707A_WORD, USB_AUX_HID_DEVICE_ID_2);
  pcm2707a_spi_send (PCM707A_WORD, USB_AUX_HID_DEVICE_ID_3);
  gpio_set(PCM2707A_CS_PORT, PCM2707A_CS_GPIO);
}