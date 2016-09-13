#include "interact.h"

// blinking LED handling
void LED_set_state(LED_state_t new_state)
{
    if (new_state == always_off)
        STATUS_LED_SetLow();
    else if (new_state == always_on)
        STATUS_LED_SetHigh();
    else if ((new_state == slow_blinking)||(new_state == fast_blinking))
    {
        STATUS_LED_SetHigh();
        LED_counter = 0;
    }
}

void LED_update_loop()
{
    if (LED_state == slow_blinking)
    {
        LED_counter++;
        if (LED_counter >= slow_blinking_period)
        {
            LED_counter = 0;
            STATUS_LED_Toggle();
        }
    }
    else if (LED_state == fast_blinking)
    {
        LED_counter++;
        if (LED_counter >= fast_blinking_period)
        {
            LED_counter = 0;
            STATUS_LED_Toggle();
        }
    }
}