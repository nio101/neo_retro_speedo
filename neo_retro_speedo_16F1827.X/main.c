/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using MPLAB(c) Code Configurator

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 3.16
        Device            :  PIC16F1827
        Driver Version    :  2.00
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.20
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "mcc_generated_files/mcc.h"
#include "main.h"
#include "interact.h"
#include "fixed_point.h"
#include "GPS.h"
#include "CRC.h"
#include "calibration.h"

// to store current speed value (unit is in m_conf.use_mph)
t_fp    speed;
//t_fp ratio_mph = convert_to_fp(0, 6210);   // ratio mph/kmh
const t_fp ratio_mph = 0x00009EF9;    // equals to 0.6210 in FP16.16
// to store current motor PWM
uint16_t motor = 0;

// EEPROM init - written once by programmer, overwritten by calibration routine
// uncomment to force calibration (due to bad crc)
//__EEPROM_DATA(0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88);

/* main */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    LED_set_state(manual_mode); // do that before activating interrupts
    STATUS_LED_SetLow();        // or random LED behaviour occurs!
    
    motor = 0;    // no PWM
    EPWM1_LoadDutyValue(1023-motor);

    TMR2_StartTimer();
    TMR0_SetInterruptHandler(my10msTimerISR);
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    // Try to load configuration/calibration from EEPROM
    if (!load_calibration_from_EEPROM())
    {
        
        LED_set_state(slow_blinking);
        button_init();
        while (m_button.confirmed_state != long_push)
        {
            if (m_button.confirmed_state == short_push)
                button_init();
        }
        perform_calibration();
        write_calibration_to_EEPROM();
    }

    GPS_Initialize();
    speed = 0;
 
    button_init();
    uint16 new_motor, speed_int;
    t_fp a, b, tmp, fp_motor;
    uint8 i;
    bool done;
    while (1)
    {
        // read/parse one NMEA VTG sentence & update speed
        if (GPS_read_speed())
        {
            // speed has been updated with the NMEA kph value
            if (m_conf.use_mph > 0) // convert to MPH if needed
                speed = multiply_fp(speed, ratio_mph);
            
            //speed = convert_to_fp(50, 0); //to debug!
            
            STATUS_LED_SetHigh();
            __delay_ms(20);
            STATUS_LED_SetLow();
            
            // now compute the new motor PWM
            speed_int = integer_part(speed);
            i = 1;
            done = false;
            while ((i<m_conf.nb_steps)&&(!done))
            {
                if (speed_int >= 10*(m_conf.nb_steps-i))
                {   // let's interpolate between i & i-1
                    if (speed_int == 10*(m_conf.nb_steps-i))
                        new_motor = m_conf.ref_pwm[i];
                    else
                    {
                        a = convert_to_fp((m_conf.ref_pwm[i-1]-m_conf.ref_pwm[i]),0);
                        tmp = convert_to_fp(0,1000); // a step is always 10
                                                 // so the inverse is 0.1
                        a = multiply_fp(a, tmp);
                        b = convert_to_fp(10*(m_conf.nb_steps-i), 0);
                        b = multiply_fp(a, b);
                        tmp = convert_to_fp(m_conf.ref_pwm[i], 0);
                        b = tmp - b;
                        // a & b are computed and pwm = a.speed + b
                        fp_motor = multiply_fp(a, speed) + b;
                        new_motor = integer_part(fp_motor);
                    }
                    done = true;
                }
                else if (i == (m_conf.nb_steps-1))
                {   // we're under 10
                    new_motor = m_conf.low_speed_pwm;
                    done = true;
                }
            }
            // apply threshold & max limit on new_motor
            if (new_motor > m_conf.max_pwm)
                new_motor = m_conf.max_pwm;
            if (new_motor < m_conf.ref_pwm[m_conf.nb_steps-1])
                new_motor = m_conf.low_speed_pwm;
            // + impulse if motor == 0
            if (motor == 0)
            {
                EPWM1_LoadDutyValue(0);
                __delay_ms(10);
            }
            // then set the new value
            motor = new_motor;
            EPWM1_LoadDutyValue(1023 - motor);
        }
        
        // check if button has been pushed
        // if long_press => calibration
        // else, reinit button
        if (m_button.confirmed_state == long_push)
        {
            motor = 0;    // no PWM
            EPWM1_LoadDutyValue(1023-motor);
            // enter calibration
            perform_calibration();
            write_calibration_to_EEPROM();            
        }
        else if (m_button.confirmed_state == short_push)
            button_init();
    }
}

void my10msTimerISR(void)
{
    LED_update_loop();
    button_update_loop();
}

/**
 End of File
*/