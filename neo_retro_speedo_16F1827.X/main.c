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
    uint8   use_mph;
    uint16  max_pwm;
    uint8   nb_steps;
    uint16  ref_pwm[20];
    uint16  low_speed_pwm;
    uint8   impulse_duration;
    uint16  crc;
} conf;

// EEPROM init - written once by programmer, overwritten by calibration routine
__EEPROM_DATA(0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88);
__EEPROM_DATA(0x49,0xC8,0x00,0x00,0x00,0x00,0x00,0x00);

/* calibration routine */

/* main */
void main(void)
{
    conf.max_pwm = 0x00;
    /*unsigned short crc = 0x11;
    crc = crc_update(crc, 0x22);
    crc = crc_update(crc, 0x33);
    crc = crc_update(crc, 0x44);
    crc = crc_update(crc, 0x55);
    crc = crc_update(crc, 0x66);
    crc = crc_update(crc, 0x77);
    crc = crc_update(crc, 0x88);*/
    // crc == 0x49C8
    
    // initialize the device
    SYSTEM_Initialize();

    LED_set_state(manual_mode); // do that before activating interrupts
    STATUS_LED_SetLow();        // or random LED behaviour occurs!
    
    TMR0_SetInterruptHandler(my10msTimerISR);
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    
    GPS_Initialize();

    speed = 0;

    while (1)
    {
        // read/parse one NMEA VTG sentence & update speed
        if (GPS_read_speed())
        {
            // speed has been updated with the NMEA kph value
            if (conf.use_mph > 0) // convert to MPH if needed
                speed = multiply_fp(speed, ratio_mph);
            
            STATUS_LED_SetHigh();
            __delay_ms(20);
            STATUS_LED_SetLow();
        }
        
        // check if button has been pushed
        // if long_press => calibration
        // else, reinit button
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