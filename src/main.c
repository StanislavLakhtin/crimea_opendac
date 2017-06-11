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
extern void encoder_setup(uint32_t *vC);
extern void encoder_handler(void);

uint32_t volumeControl = SYSTEM_TICKS_PER_SEC;

#define STACK_SIZE      1024
#define THREAD_PRIO     42

static ATOM_TCB main_tcb;

static uint8_t thread_stacks[2][STACK_SIZE];

static void main_thread_func(uint32_t data);

void exti_setup(void) {
  nvic_enable_irq(NVIC_EXTI9_5_IRQ);
  exti_enable_request(EXTI8);
  exti_set_trigger(EXTI8, EXTI_TRIGGER_FALLING);
  exti_select_source(EXTI8, GPIOA);
}

void exti9_5_isr(void) {
  encoder_handler();
  exti_reset_request(EXTI8); // нам следует вручную очистить причину вызова прерывания
}

int main(void) {
  int8_t status;
  uint32_t loop;

  /**
   * Brief delay to give the debugger a chance to stop the core before we
   * muck around with the chip's configuration.
   */
  for (loop = 0; loop < 1000000; ++loop) {
    __asm__("nop");
  }

  board_setup();
  exti_setup();
  encoder_setup(&volumeControl);

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
  /* Print message */
  ssd1306_drawWCharStr(0,0,white, wrapDisplay,L"Привет! Это Крым.\nНачинаем.");
  ssd1306_refresh();
  uint32_t loop;
  for (loop = 0; loop < 1000000; ++loop) {
    __asm__("nop");
  }
  ssd1306_drawWCharStr(0,16,white,nowrap,L"Подтяжка USB включена");
  pullUp_ON();
  ssd1306_refresh();
  /* Loop forever and blink the LED */
  while (1) {
    atomTimerDelay(volumeControl);
  }

}

