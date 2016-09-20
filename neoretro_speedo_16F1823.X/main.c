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
        Device            :  PIC16F1823
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
#include "interact.h"
#include "fixed_point.h"
/*
 * Helpers
 */

#define NMEA_BUFFER_SIZE 6

void my10msTimerISR(void);  // custom function called every 10ms,
                            // used for LED blinking and push button debounce

#define __delay_sec(x) for(unsigned char tmp=0;tmp<(10*x);tmp++){__delay_ms(100);}

// to store current speed value and units
typedef enum _speed_unit_type { MPH = 0, KPH } speed_unit_type;
speed_unit_type speed_units = MPH;  // MPH by default
t_fp    speed;
//t_fp ratio_mph = convert_to_fp(0, 6210);   // ratio mph/kmh
const t_fp ratio_mph = 0x0027BE;    // equals to 0.6210 in FP10.14

// to store speedometer calibration
//[type?] Pcal[13]; // stores percentage calibration values for
                  // speeds @ 10,20,30..150 in [speed_unit_type]
                  // type should be the same as used by PWM,
                  // value of XXX => no calibration available
void GPS_Initialize(void)
{
    __delay_sec(1); // needed for letting the GPS start
    // filter the NMEA sentences => only the VTG (speed)@every fix
    printf("$PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
    __delay_ms(100);
    printf("$PMTK220,100*2F\r\n"); // ask for a fix every 100ms (update @10hz)
    __delay_ms(100);
}

bool GPS_read_speed(void) // read & parse/check one NMEA sentence
{   
    // example of VTG sentence:
    // $GPVTG,165.48,T,,M,150.03,N,300.06,K,A*37
    unsigned char buffer[NMEA_BUFFER_SIZE];
    unsigned char i_buff = 0;
    unsigned char checksum = 0x00;
    // we are going to wait for a '$' to begin recording received chars
    unsigned char m_char = 0x00;
    while (m_char != '$')
        if(eusartRxCount!=0)
            m_char=EUSART_Read();  // read a byte from RX
    
    // were are at the beginning of a new NMEA sentence
    // let's get the NMEA header and check that it's a VTG sentence
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    // buffer should contain 'GPVTG'...
    if ((buffer[0] != 'G')||(buffer[1] != 'P')||(buffer[2] != 'V')||(buffer[3] != 'T')||(buffer[4] != 'G'))
        return false;
    i_buff = 0; // clear buffer
    
    // let's wait and then skip the T field...
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
        }
    // should be the T keyword here
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[0] != 'T')||(i_buff != 1))
        return false;
    i_buff = 0; // clear buffer

    // let's wait and then skip the M field...
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
        }
    // should be the M keyword here
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[0] != 'M')||(i_buff != 1))
        return false;
    i_buff = 0; // clear buffer
    
    // let's wait and then skip the N field...
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
        }
    // should be the N keyword here
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[0] != 'N')||(i_buff != 1))
        return false;
    i_buff = 0; // clear buffer
    
    // let's wait and then read the K field...
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    // buffer should contain the current speed reading in km/h
    // read it now and convert it to speed/FP
    unsigned char i_buff2 = 0;
    unsigned short read_speed_int=0;
    unsigned short read_speed_fra=0;
    bool before_dot = true;
    while (i_buff2 < i_buff)
        if (buffer[i_buff2]=='.')
        {
            before_dot = false;
            i_buff2++;  // step over the '.'
        }
        else
        {
            if ((buffer[i_buff2]<0x30)||(buffer[i_buff2]>0x39))
                return false;   // not a digit, abort!
            else
            {
                if (before_dot)
                    read_speed_int = read_speed_int*10 + (buffer[i_buff2]-0x30);
                else
                    read_speed_fra = read_speed_fra*10 + (buffer[i_buff2]-0x30);
                i_buff2++;
            }
        }
    // this type of GPS always outputs 2 digits avec dot
    // so multiply the fractional part by 100
    // to comply with the FP definition/conversion
    read_speed_fra *= 100;
    // read_speed_int/fra now contain the read speed
    // if checksum is ok, overwrite speed later
    i_buff = 0; // clear buffer
    
    // should be the K keyword here
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            checksum ^= m_char;
            if (m_char == ',')
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[0] != 'K')||(i_buff != 1))
        return false;
    i_buff = 0; // clear buffer
    
    // we should now arrive to the last field + checksum value
    bool no_more_checksum = false;
    while (1)
        if (eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte from RX
            if (m_char == '*')
                no_more_checksum = true;
            if (!no_more_checksum)
                checksum ^= m_char;
            if (m_char == 0x0D)
                break;
            else
            {
                buffer[i_buff++] = m_char;
                if (i_buff >= NMEA_BUFFER_SIZE)
                    return false; // field too long
            }
        }
    if ((buffer[1] != '*')||(i_buff != 4))
        return false;

    // now check the checksum against ours
    unsigned char checksum2 = 0;
    if (buffer[2] > 0x39)   // not 0-9, more A-F
        checksum2 = buffer[2] - 0x37;
    else
        checksum2 = buffer[2] - 0x30;
    checksum2 = checksum2 << 4;
    if (buffer[3] > 0x39)   // not 0-9, more A-F
        checksum2 += buffer[3] - 0x37;
    else
        checksum2 += buffer[3] - 0x30;
    if (checksum == checksum2)
    {
        speed = convert_to_fp(read_speed_int, read_speed_fra);
        return true;
    }
    else
        return false;
}

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    TMR0_SetInterruptHandler(my10msTimerISR);
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();
    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    LED_set_state(manual_mode);
    STATUS_LED_SetLow();
    __delay_sec(3);

    GPS_Initialize();

    speed = 0;

    while (1)
    {
        // read/parse one NMEA sentence & update speed
        if (GPS_read_speed())
        {
            // speed has been updated with the NMEA kph value
            if (speed_units == MPH) // convert to MPH if needed
                speed = multiply_fp(speed, ratio_mph);
            
            STATUS_LED_SetHigh();
            __delay_ms(20);
        }
        STATUS_LED_SetLow();
        
        // check if button has been pushed
        // if long_press => calibration
        // else, reinit button
    }
    
    // test PWM now!
    
    //uint16_t motor_load = 128; // == 0-1023 for 100-0% motor load (inverted))
    uint16_t motor_load = 0;
    EPWM_LoadDutyValue(1023-motor_load);
    TMR2_StartTimer();
    __delay_sec(5);
    
    bool up = true;
    
    // test loop, slowly from 0 to --- and back to 0
    /*
    while (1)
    {
        __delay_ms(200);
        if (up == true)
            motor_load += 1;
        else
            motor_load -= 1;
        if (motor_load > 450)   //730
            up = false;
        else if (motor_load == 0)
            up = true;
        EPWM_LoadDutyValue(1023-motor_load);
    }*/
    
    // simulate use with impulse+threshold under 10MPH value
    
    EPWM_LoadDutyValue(0);  // impulse to start
    __delay_ms(10);
    EPWM_LoadDutyValue(1023-148);   // 10MPH
    __delay_sec(5);
    while (1)
    {
        if (up == true)
            motor_load += 1;
        else
            motor_load -= 1;
        if (motor_load > 300)
            up = false;
        else if (motor_load == 0)
        {
            up = true;
            EPWM_LoadDutyValue(1023);
            __delay_sec(5);
            // impulsion pour décoller!
            EPWM_LoadDutyValue(0);
            __delay_ms(10);
        }
        if (motor_load < 148)   // threshold en dessous de 10MPH
            EPWM_LoadDutyValue(1023-135);//148-%
        else
            EPWM_LoadDutyValue(1023-motor_load);
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