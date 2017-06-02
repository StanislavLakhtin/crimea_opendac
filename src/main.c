//
// Created by sl on 18.02.17.
//

#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <atom.h>
#include <atomport.h>
#include <atomport-private.h>
#include <atomtimer.h>

#define STACK_SIZE      1024
#define THREAD_PRIO     42

static ATOM_TCB main_tcb;

static uint8_t thread_stacks[2][STACK_SIZE];

static void main_thread_func(uint32_t data);

/**
 * Example for a stand-alone board application
 */
extern int board_setup(void);
int main(void)
{
    int8_t status;
    uint32_t loop;

    /**
     * Brief delay to give the debugger a chance to stop the core before we
     * muck around with the chip's configuration.
     */
    for(loop = 0;loop < 1000000;++loop){
        __asm__("nop");
    }

    board_setup();

    /**
     * Initialise OS and set up idle thread
     */
    status = atomOSInit(&thread_stacks[0][0], STACK_SIZE, FALSE);

    if(status == ATOM_OK){
        /* Set up main thread */
        status = atomThreadCreate(&main_tcb, THREAD_PRIO, main_thread_func, 0,
                                  &thread_stacks[1][0], STACK_SIZE, TRUE);

        if(status == ATOM_OK){
            atomOSStart();
        }
    }

    while(1)
        ;

    /* We will never get here */
    return 0;
}

extern void test_led_toggle(void);
static void main_thread_func(uint32_t data __maybe_unused)
{
    /* Print message */
    printf("Hello, world!\n");

    /* Loop forever and blink the LED */
    while(1){
        test_led_toggle();
        atomTimerDelay(SYSTEM_TICKS_PER_SEC);
    }

}

