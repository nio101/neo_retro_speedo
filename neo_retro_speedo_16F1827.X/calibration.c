/* 
 * File:   calibration.c
 * Author: nio
 *
 * Created on 22 septembre 2016, 11:39
 */

#include "calibration.h"
#include "CRC.h"
#include "interact.h"

// Configuration/calibration data
// use_mph (bool) - 1 byte
// max_pwm: maximum usable pwm - 2 bytes
// number of steps(max 20) / 3 means steps @(30,20,10) - 1 byte
// ref_pwm[nb_steps] : 2 bytes x nb_steps
// ref_pwm for low speed: 2 bytes
// impulse duration for low speed: duration in ms - 1 byte
// CRC for previous data: 

uint16  m_crc;

/* calibration routines */
bool load_calibration_from_EEPROM()
{
    // let's try to read the m_conf struct from EEPROM
    uint8 addr = 0x00;
    uint16 crc = 0x00;
    char i,*p; 
    p=(char *)&m_conf;
    for(i=0; i<sizeof(m_conf); i++)
    {
        *p = DATAEE_ReadByte(addr++);
        crc = crc_update(crc, *p++);
    }
    p=(char *)&m_crc;
    for(i=0; i<sizeof(m_crc); i++)
        *p++ = DATAEE_ReadByte(addr++);
    return (crc == m_crc);
}

void write_calibration_to_EEPROM()
{
    // let's dump the m_conf struct to EEPROM
    uint8 addr = 0x00;
    m_crc = 0x00;
    char i,*p; 
    p=(char *)&m_conf;  // char pointer on conf struct
    for(i=0; i<sizeof(m_conf); i++) {   // about 49 bytes...
        DATAEE_WriteByte(addr++, *p);
        m_crc = crc_update(m_crc, *p++);
    }
    p=(char *)&m_crc;
    for(i=0; i<sizeof(m_crc); i++)
        DATAEE_WriteByte(addr++, *p++);
}

void perform_calibration()  // TODO: add low_speed+impulse settings
{
    LED_set_state(manual_mode);
    multiple_fast_blink(2);
    __delay_sec(1);
    button_init();
 
    // step 1: MPH/KPH
    LED_set_state(always_on);
    m_conf.use_mph = 0;
    while (m_button.confirmed_state != long_push)
    {
        if (m_button.confirmed_state == short_push)
        {
            m_conf.use_mph = !m_conf.use_mph;
            if (m_conf.use_mph)
                LED_set_state(slow_blinking);
            else
                LED_set_state(fast_blinking);
            button_init();
        }
    }
    multiple_fast_blink(2);
    button_init();
    
    // step 2: max PMW
    uint16 m_motor = 200;
    // impulse to get started
    EPWM1_LoadDutyValue(0);
    __delay_ms(30);
    // don't use debounce feature here to reduce latency
    // any push will do
    while (1)
    {
        if (get_button_state() && (m_motor >= 300))
            break;
        m_motor++;
        if (m_motor == 1023)
            m_motor = 300;
        EPWM1_LoadDutyValue(1023-m_motor);
        __delay_ms(30);
    }
    m_conf.max_pwm = m_motor;
    multiple_fast_blink(2);
    __delay_sec(1);
    //button_init();
    
    // step 3: from max RPM down to 0, with every 10th selected
    bool done = false;
    while (!done)
    {
        uint8 i = 0;
        m_motor = m_conf.max_pwm;
        while (m_motor > 100)
        {
            if (get_button_state())
            { // record the value
              m_conf.ref_pwm[i++] = m_motor;
              multiple_fast_blink(2);
              __delay_sec(1);
            }
            m_motor--;
            EPWM1_LoadDutyValue(1023-m_motor);
            __delay_ms(150);
        }
        m_conf.nb_steps = i;
        EPWM1_LoadDutyValue(1023-0);
        multiple_fast_blink(2);
        __delay_sec(1);
        // replay the steps
        bool replay = true;
        button_init();
        while (replay)
        {
            for (i=0; i<m_conf.nb_steps; i++)
            {
                EPWM1_LoadDutyValue(1023-m_conf.ref_pwm[i]);
                __delay_sec(3);
                if (m_button.confirmed_state != nothing)
                {
                    replay = false;
                    break;
                }
            }
            // short press to restart, long press to confirm
            if (m_button.confirmed_state == long_push)
                done = true;
        }
    }
    multiple_fast_blink(2);
    __delay_sec(1);
    button_init();
    
    // step 4: settings for start impulse
    m_conf.impulse_duration = 10;   // 10ms
    
    /*uint8 values_impulse[5] = {20, 15, 10, 5, 2};
    uint8 i = 0;
    
    bool done = false;
    button_init();
    while (!done)
    {
        multiple_fast_blink(i+1);
        EPWM1_LoadDutyValue(1023-0); // no PWM
        __delay_sec(1);
        m_conf.impulse_duration = values_impulse[i];
        EPWM1_LoadDutyValue(0);
        delay_ms(m_conf.impulse_duration);
        EPWM1_LoadDutyValue(1023-m_conf.ref_pwm[m_conf.nb_steps-1]); // 10'
        __delay_sec(3);
        // short press to next value, long press to confirm
        if (m_button.confirmed_state == long_push)
            done = true;
        else if (m_button.confirmed_state == short_push)
        {
            i++;
            if (i>=5)
                i = 0;
            button_init();
        }
    }
    
    multiple_fast_blink(2);
    __delay_sec(1);
    button_init();
    */

    // step 5: settings for speed under 10MPH/KPH
    m_conf.low_speed_pwm = m_conf.ref_pwm[m_conf.nb_steps-1] - 10;
    
    /*uint8 values_low[5] = {4, 8, 12, 16, 20};
    i = 0;
    bool done = false;
    while (!done)
    {
        multiple_fast_blink(i+1);
        EPWM1_LoadDutyValue(1023-0); // no PWM
        __delay_sec(1);
        m_conf.low_speed_pwm = m_conf.ref_pwm[m_conf.nb_steps-1] - values_low[i];
        EPWM1_LoadDutyValue(0);
        delay_ms(m_conf.impulse_duration);
        EPWM1_LoadDutyValue(1023-m_conf.low_speed_pwm);
        button_init();
        while (m_button.confirmed_state == nothing)
        {}
        if (m_button.confirmed_state == short_push)
        {
            i++;
            if (i>=5)
                i = 0;
        }
        else if (m_button.confirmed_state == long_push)
            done = true;
    }
    multiple_fast_blink(2);
    __delay_sec(1);
    button_init();
    */
        
    // exiting
    LED_set_state(manual_mode); // do that before activating interrupts
    STATUS_LED_SetLow();
    EPWM1_LoadDutyValue(1023-0); // no PWM
}