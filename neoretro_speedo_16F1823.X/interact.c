#include "interact.h"

// ----------------------------------------------------------------------------
// blinking LED handling
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// push button handling
// ----------------------------------------------------------------------------

void button_init()
{
    b_last_state = BUTTON_GetValue();
    b_count_pressed = 0;
    b_count_released = 0;
    b_confirmed_state = nothing;
    b_push_confirmed = false;
    b_release_confirmed = false;
    b_has_once_been_released = false;
}

void button_update_loop()
{
    unsigned char current_state = BUTTON_GetValue();
    if (!b_push_confirmed)
    {
        // button state has changed while push was not confirmed
        if (current_state != b_last_state)
        {
            b_count_pressed = 0;
            b_last_state = current_state;
        }
        else if (b_last_state == 1)
        {   // button is still in same pushed state
            b_count_pressed++;
            if (b_count_pressed > count_short_push) // !<255, b_count is uint8
                b_push_confirmed = true;
        }
    }
    else    // b_push_confirmed is true here
    {
        if (current_state != b_last_state)
        {
            b_count_released = 0;
            b_last_state = current_state;
            b_has_once_been_released = true;
        }
        else if ((b_last_state == 1)&&(!b_has_once_been_released))
            b_count_pressed++;            
        else if (b_last_state == 0)
        {   // button is in the same released state
            b_count_released++;
            if (b_count_released > count_release) // !<255, b_count is uint8
                if (b_count_pressed >= count_long_push)
                    b_confirmed_state = long_push;
                else
                    b_confirmed_state = short_push;
        }
    }
}
