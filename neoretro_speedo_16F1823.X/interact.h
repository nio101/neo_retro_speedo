#include "mcc_generated_files/mcc.h"

// types for push button handling
typedef enum {
    unknown, short_push, long_push
} confirmed_state_t;

// types for blinking LED handling
typedef enum {
    always_on, always_off, slow_blinking, fast_blinking
} LED_state_t;

extern LED_state_t LED_state;
extern unsigned char volatile LED_counter;

#define slow_blinking_period 100
#define fast_blinking_period 50

void LED_set_state(LED_state_t new_state);
void LED_update_loop();
