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


void my10msTimerISR(void);  // custom function called every 10ms,
                            // used for LED blinking and push button debounce

// to store current speed value and units
//typedef enum _speed_unit_type { MPH = 0, KPH } speed_unit_type;
//speed_unit_type speed_units = MPH;  // MPH by default
t_fp    speed;
//t_fp ratio_mph = convert_to_fp(0, 6210);   // ratio mph/kmh
const t_fp ratio_mph = 0x00009EF9;    // equals to 0.6210 in FP16.16
uint16_t motor = 0;

// Configuration/calibration data
// use_mph (bool) - 1 byte
// max_pwm: maximum usable pwm - 2 bytes
// number of steps(max 20) / 3 means steps @(30,20,10) - 1 byte
// ref_pwm[nb_steps] : 2 bytes x nb_steps
// ref_pwm for low speed: 2 bytes
// impulse duration for low speed: duration in ms - 1 byte
// CRC for previous data: 

struct conf_data
{
    bool   use_mph;
    uint16  max_pwm;
    uint8   nb_steps;
    uint16  ref_pwm[20];
    uint16  low_speed_pwm;
    uint8   impulse_duration;
} m_conf;

uint16  m_crc;

// EEPROM init - written once by programmer, overwritten by calibration routine
//__EEPROM_DATA(0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88);
//__EEPROM_DATA(0x49,0xC8,0x00,0x00,0x00,0x00,0x00,0x00);

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

void perform_calibration()
{
    double_fast_blink();
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
    double_fast_blink();
    button_init();
    
    // step 2: max PMW
    motor = 200;
    // impulse to get started
    EPWM1_LoadDutyValue(0);
    __delay_ms(30);
    // don't use debounce feature here to reduce latency
    // any push will do
    while (1)
    {
        if (get_button_state() && (motor >= 300))
            break;
        motor++;
        if (motor == 1023)
            motor = 300;
        EPWM1_LoadDutyValue(1023-motor);
        __delay_ms(30);
    }
    m_conf.max_pwm = motor;
    double_fast_blink();
    __delay_sec(1);
    //button_init();
    
    // step 3: from max RPM down to 0, with every 10th selected
    bool done = false;
    while (!done)
    {
        uint8 i = 0;
        motor = m_conf.max_pwm;
        while (motor > 100)
        {
            if (get_button_state())
            { // record the value
              m_conf.ref_pwm[i++] = motor;
              double_fast_blink();
              __delay_sec(1);
            }
            motor--;
            EPWM1_LoadDutyValue(1023-motor);
            __delay_ms(150);
        }
        m_conf.nb_steps = i;
        EPWM1_LoadDutyValue(1023-0);
        double_fast_blink();
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
    double_fast_blink();
    __delay_sec(1);
    button_init();
    
    // step 4 : settings for speed under 10MPH/KPH
    m_conf.low_speed_pwm = m_conf.ref_pwm[m_conf.nb_steps-1] - 10; // or apply
                                                                   // -X%?
    m_conf.impulse_duration = 10; // 10 msec
    
    // exiting
    LED_set_state(manual_mode); // do that before activating interrupts
    STATUS_LED_SetLow();
    EPWM1_LoadDutyValue(1023-0); // no PWM
}

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
            // + impulse if motor == 0
            // then set the new value
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
        
    bool up = true;
    
    // test loop, slowly from 0 to --- and back to 0
    
    /*while (1)
    {
        __delay_ms(200);
        if (up == true)
            motor_load += 1;
        else
            motor_load -= 1;
        if (motor_load > 900)   //730
            up = false;
        else if (motor_load == 0)
            up = true;
        EPWM1_LoadDutyValue(1023-motor_load);
    }*/
    
    // simulate use with impulse+threshold under 10MPH value
    
    EPWM1_LoadDutyValue(0);  // impulse to start
    __delay_ms(10);
    EPWM1_LoadDutyValue(1023-148);   // 10MPH
    __delay_sec(5);
    while (1)
    {
        if (up == true)
            motor += 1;
        else
            motor -= 1;
        if (motor > 300)
            up = false;
        else if (motor == 0)
        {
            up = true;
            EPWM1_LoadDutyValue(1023);
            __delay_sec(5);
            // impulsion pour décoller!
            EPWM1_LoadDutyValue(0);
            __delay_ms(10);
        }
        if (motor < 148)   // threshold en dessous de 10MPH
            EPWM1_LoadDutyValue(1023-135);//148-1X%
        else
            EPWM1_LoadDutyValue(1023-motor);
        __delay_ms(50);
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