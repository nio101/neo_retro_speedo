/* 
 * File:   fixed_point.h
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */

#include "interact.h"

// ----------------------------------------------------------------------------
// blinking LED handling
// ----------------------------------------------------------------------------

void LED_set_state(LED_state_t new_state)
{
    m_LED.state = new_state;
    m_LED.counter = 0;
    if (new_state == always_off)
        STATUS_LED_SetLow();
    else if (new_state == always_on)
        STATUS_LED_SetHigh();
    else if ((new_state == slow_blinking)||(new_state == fast_blinking))
        STATUS_LED_SetHigh();
}

void LED_update_loop()
{
    if (m_LED.state == manual_mode)
        return;
    else if (m_LED.state == slow_blinking)
    {
        m_LED.counter++;
        if (m_LED.counter >= slow_blinking_period)
        {
            m_LED.counter = 0;
            STATUS_LED_Toggle();
        }
    }
    else if (m_LED.state == fast_blinking)
    {
        m_LED.counter++;
        if (m_LED.counter >= fast_blinking_period)
        {
            m_LED.counter = 0;
            STATUS_LED_Toggle();
        }
    }
}

// ----------------------------------------------------------------------------
// push button handling
// ----------------------------------------------------------------------------

unsigned char get_button_state()
{
    // ! warning ! button state is pulled-up, it reads 0/GND in pushed state
    unsigned char state = BUTTON_GetValue();
    if (state == 1)
        return 0;
    else return 1;
}

void button_init()
{
    m_button.last_state = get_button_state();
    m_button.count_pressed = 0;
    m_button.count_released = 0;
    m_button.confirmed_state = nothing;
    m_button.push_confirmed = false;
    m_button.release_confirmed = false;
    m_button.has_once_been_released = false;
}

void button_update_loop()
{
    unsigned char current_state = get_button_state();
    if (!m_button.push_confirmed)
    {
        // button state has changed while push was not confirmed
        if (current_state != m_button.last_state)
        {
            m_button.count_pressed = 0;
            m_button.last_state = current_state;
        }
        else if (m_button.last_state == 1)
        {   // button is still in same pushed state
            m_button.count_pressed++;
            if (m_button.count_pressed > count_short_push) // !<255, b_count is uint8
                m_button.push_confirmed = true;
        }
    }
    else    // b_push_confirmed is true here
    {
        if (current_state != m_button.last_state)
        {
            m_button.count_released = 0;
            m_button.last_state = current_state;
            m_button.has_once_been_released = true;
        }
        else if ((m_button.last_state == 1)&&(!m_button.has_once_been_released))
        {
            m_button.count_pressed++;
            if (m_button.count_pressed >= max_count_push)
                m_button.confirmed_state = long_push;
        }            
        else if (m_button.last_state == 0)
        {   // button is in the same released state
            m_button.count_released++;
            if (m_button.count_released > count_release) // !<255, b_count is uint8
                if (m_button.count_pressed >= count_long_push)
                    m_button.confirmed_state = long_push;
                else
                    m_button.confirmed_state = short_push;
        }
    }
}
