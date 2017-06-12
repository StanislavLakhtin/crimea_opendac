//
// Created by sl on 18.02.17.
//

#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>

#include <atom.h>
#include <ssd1306_i2c.h>
#include "crimea_dac.h"

/**
 * External declarations
 */
extern int board_setup(void);
extern void display_WakeUp(void);
extern void encoder_setup(volatile uint8_t *vC);
extern void encoder_handler(void);

volatile uint8_t volumeControl = 5;

#define STACK_SIZE      1024
#define THREAD_PRIO     42

static ATOM_TCB main_tcb;

static uint8_t thread_stacks[2][STACK_SIZE];

static void main_thread_func(uint32_t data);

void exti_setup(void) {
  nvic_enable_irq(NVIC_EXTI3_IRQ);
  exti_enable_request(EXTI3);
  exti_set_trigger(EXTI3, EXTI_TRIGGER_FALLING);
  exti_select_source(EXTI3, GPIOA);

  nvic_enable_irq(NVIC_EXTI2_IRQ);
  exti_enable_request(EXTI2);
  exti_set_trigger(EXTI2, EXTI_TRIGGER_FALLING);
  exti_select_source(EXTI2, GPIOA);
}

void exti2_isr(void) {
  encoder_handler();
  exti_reset_request(EXTI2); // нам следует вручную очистить причину вызова прерывания
}

void exti3_isr(void) {
  encoder_handler();
  exti_reset_request(EXTI3); // нам следует вручную очистить причину вызова прерывания
}

int main(void) {
  int8_t status;

  board_setup();
  encoder_setup(&volumeControl);
  exti_setup();
  /* Print message */
  ssd1306_drawWCharStr(0,0,white, wrapDisplay,L"Привет! Это Крым.");
  pcm2707a_busReset();

  if (!gpio_get(PCM2707A_SUSPEND_PORT, PCM2702A_SUSPEND_GPIO))
    ssd1306_drawWCharStr(0,8,white, wrapDisplay,L"PCM2707 ~SSPND is LOW");
  else
    ssd1306_drawWCharStr(0,8,white, wrapDisplay,L"PCM2707 ~SSPND is HIGH");
  pcm2707a_setup();
  exti_setup();
  encoder_setup(&volumeControl);
  ssd1306_refresh();
  /**
   * Initialise OS and set up idle thread
   */
  status = atomOSInit(&thread_stacks[0][0], STACK_SIZE, FALSE);

  if (status == ATOM_OK) {
    /* Set up main thread */
    status = atomThreadCreate(&main_tcb, THREAD_PRIO, main_thread_func, 0,
                              &thread_stacks[1][0], STACK_SIZE, TRUE);

    if (status == ATOM_OK) {
      atomOSStart();
    }
  }

  while (1);

  /* We will never get here */
  return 0;
}

static void main_thread_func(uint32_t data __maybe_unused) {
  uint8_t vC = volumeControl;
  wchar_t buffer[40];

  /* Loop forever and blink the LED */
  while (1) {
    if (vC!=volumeControl) {
      ssd1306_clear();
      vC = volumeControl;
      swprintf(buffer, 40, L"%d", vC);
      ssd1306_drawWCharStr(0,8,white, nowrap, L"Volume:");
      ssd1306_drawWCharStr(0,16,white, nowrap, buffer);
      ssd1306_refresh();
    }
    atomTimerDelay(SYSTEM_TICKS_PER_SEC);
  }
}