/* 
 * File:   interact.h
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "mcc_generated_files/mcc.h"


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

LED_state_t LED_state=always_off;
volatile unsigned char LED_counter=0;

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

typedef enum {
    nothing, short_push, long_push
} confirmed_state_t;
volatile confirmed_state_t b_confirmed_state;

volatile unsigned char b_count_pressed;
volatile unsigned char b_count_released;
volatile bool b_push_confirmed;
volatile bool b_release_confirmed;
volatile unsigned b_last_state;
volatile bool b_has_once_been_released;

void button_init();
void button_update_loop();
unsigned char get_button_state();