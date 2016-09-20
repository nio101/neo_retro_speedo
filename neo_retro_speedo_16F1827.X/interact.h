/* 
 * File:   interact.h
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */

#ifndef INTERACT_H
#define	INTERACT_H

#include "mcc_generated_files/mcc.h"
#include "main.h"

// ----------------------------------------------------------------------------
// blinking LED handling
// ----------------------------------------------------------------------------

// basic timer-based LED blinking status
// important: LED_update_loop() must be called in a 10ms interrupt loop!
// usage:
// 1) LED_set_state([LED_state]);

typedef enum {
    always_on, always_off, slow_blinking, fast_blinking, manual_mode
} LED_state_t;

volatile struct {
    volatile LED_state_t state;
    volatile uint8 counter;
} m_LED;

#define slow_blinking_period 85
#define fast_blinking_period 20

void LED_set_state(LED_state_t new_state);
void LED_update_loop();

// ----------------------------------------------------------------------------
// push button handling
// ----------------------------------------------------------------------------

// button handling consists here of detecting button long & short push
// using a basic debouncing delay on any event
// important: button_update_loop() must be called in a 10ms interrupt loop!
// usage is:
// 1) button_init()
// 2) while (b_confirmed_state == nothing)
// { do something while waiting for button push detection; }

// 50ms min for a short push
#define count_short_push 5
// 750s min for a long push
#define count_long_push 75
// 1s push time max
#define max_count_push 100
// 100ms min for a release event
#define count_release   10

volatile struct {
    volatile enum {
        nothing, short_push, long_push
    } confirmed_state;
    volatile uint8 count_pressed;
    volatile uint8 count_released;
    volatile bool push_confirmed;
    volatile bool release_confirmed;
    volatile uint8 last_state;
    volatile bool has_once_been_released;
} m_button;

void button_init();
void button_update_loop();
uint8 get_button_state();

#endif	/* INTERACT_H */