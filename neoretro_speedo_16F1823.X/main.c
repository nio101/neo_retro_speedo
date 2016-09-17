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
unsigned short speed_int = 0;
unsigned short speed_dec = 0;

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
    
    // let's wait and then skip the K field...
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
    // we should read it! :)
    unsigned char tmp = 0;
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
    return true;
}

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    //SYSTEM_Initialize();
    //TMR0_SetInterruptHandler(my10msTimerISR);
 
    // test FP
    //t_fp fp1 = convert_to_fp(0, 621);   // ratio mph/kmh
    //t_fp read_speed_kph = convert_to_fp(130, 620);
    t_fp test = convert_to_fp(23, 6875);
    //t_fp speed_mph = multiply_fp(fp1, read_speed_kph);
    char zob = 'k';
    //unsigned long short f2 = toFP(2);
    
    //unsigned long short fp_res = mult(f1, f2);
    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    // pour le PWM: prendre FOSC==32 MHz, PWMFreq==31.25 kHz
    // => PR2=0xFF && TMR2 pre-scaler=1:1
    // et du coup on a une résolution PWM de 10bits
    // + datasheet p196 pour procédure init propre du PWM
    
    // pour l'UART: 9600 baud, 8data bits, no parity
    // 1 stop bit, receive polarity: idle1
    // output: normal (GND/3.3V)
    
    // EEPROM
    
    // pushbutton + debounce timer
    
    // NMEA: $VTG,,,,,*[checksum]<CR><LF>
    // CR:0x0D LF:0x0A
    // max length: 79 chars between '$' and "<CR><LF>"
    // Utiliser $ pour détecter le début de trame, commencer réception
    // si $ => retour début de trame/réinit réception
    // si <LF> => fin réception donc analyse commande, checksum,
    // et lecture paramètres
    // Si buffer déborde => ignore buffer & stop réception.
    
    // printf de configuration du GPS:
    
    // API_SET_NMEA_OUTPUT pour filtrer les messages:
    // $PMTK314,0,0,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C<CR><LF>
    // => VTG every fix, GGA every 5 fixes, nothing else
    // $PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29<CR><LF>
    // => VTG every fix, nothing else)
    
    // SET UPDATE FREQ
    // $PMTK220,100*2F<CR><LF>
    // => position fix done 10 times per second (instead of 1 per second)

    // 1ère étape: laisser faire l'UART initialize avec les printf
    // et mettre le bus pirate en copie de ce que reçoit le pic
    // => tester reset, gain de fix & perte de fix
    
    // 2ème étape: lire les trames NMEA, éventuellement updater la vitesse
    // et la passer à zero si pas de trame récente (pour éviter de rester
    // scotché en cas de perte de fix)
    /*while(1)
    {
        // Test DA LED output! :)
         LED_set_state(slow_blinking);
         __delay_sec(5);
         LED_set_state(fast_blinking);
         __delay_sec(5);
         LED_set_state(always_on);
         __delay_sec(5);
         LED_set_state(always_off);
         __delay_sec(5);
    }*/        

    LED_set_state(manual_mode);
    STATUS_LED_SetLow();
    __delay_sec(3);
    
    /*while(1)
    {
        __delay_sec(5);
        // Test DA button now!
        LED_set_state(always_on);
        button_init();
        while (b_confirmed_state == nothing)
        {}

        if (b_confirmed_state == short_push)
            LED_set_state(fast_blinking);
        else if (b_confirmed_state == long_push)
            LED_set_state(slow_blinking);
    }*/    
    
    GPS_Initialize();

    // test NMEA speed reading!
    bool res;
    while (1)
    {
        res = GPS_read_speed(); // read/parse one NMEA sentence
        if (res)
        {
            STATUS_LED_SetHigh();
            __delay_ms(20);
        }
        STATUS_LED_SetLow();
        
        // check if last VTG data is old (> how many sec?)
        // if so, we may have lost the fix => PWM to zero?
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
    
    /*while (1)
    {
        GPS_read_speed(); // read/check one NMEA sentence
        // check if last VTG data is old (> how many sec?)
        // if so, we may have lost the fix => PWM to zero?
    }*/
}

void my10msTimerISR(void)
{
    LED_update_loop();
    button_update_loop();
}

/**
 End of File
*/

/*
    // TODO: ne pas bufferiser & parser la chaine complète, mais
    // détecter les changements de champs au travers d'états dédiés
    // + calculer checksum au fil de l'eau
    // sinon pas assez de RAM pour faire la calibration... :s
    
    // Part 1/2: let's record a complete NMEA sentence
    
    unsigned char buffer[NMEA_BUFFER_SIZE];
    unsigned char i_buff = 0;
    // we are going to wait for a '$' to begin recording received chars
    // until another '$' comes (restart recording), or an <LF> (parse sentence)
    // if the sentence is a correct VTG, read the speed and reset dedicated tmr
    bool done = false;
    bool recording = false;
    unsigned char m_char;
    while (!done)
    {
        if(eusartRxCount==0)
        {
            //STATUS_LED_SetLow();
        }        
        else if(eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte for RX
            //STATUS_LED_SetHigh();
            //__delay_ms(1);
            if (m_char == '$')
            {
                recording = true;
                i_buff = 0;
            }
            else if (recording)
            {
                if (m_char == 0x0D) // <CR>
                    done = true;
                else
                    buffer[i_buff++] = m_char;
                if (i_buff > NMEA_BUFFER_SIZE)
                {
                    // NMEA sentence is too long for buffer!
                    // ignore it, then to avoid critical error
                    recording = false;
                }
            }
        }
    }
    
    unsigned char i_buff2 = 0;
    unsigned short read_speed_int = 0;
    unsigned short read_speed_dec = 0;
    // Part 2/2: parse the NMEA sentence
    if ((buffer[i_buff2++] == 'G')&&(buffer[i_buff2++] == 'P')&&(buffer[i_buff2++] == 'V')&&(buffer[i_buff2++] == 'T')&&(buffer[i_buff2++] == 'G'))
    {
        // T field
        while ((i_buff2 < i_buff)&&(buffer[i_buff2]!=','))  //reach the next ','
            i_buff2++;
        if (i_buff2 >= i_buff)
            return false;
        i_buff2++; // step over the ','
        if (buffer[i_buff2++] != 'T')
            return false;
        i_buff2++; // step over the ','

        // M field
        while ((i_buff2 < i_buff)&&(buffer[i_buff2]!=','))  //reach the next ','
            i_buff2++;
        if (i_buff2 >= i_buff)
            return false;
        i_buff2++; // step over the ','
        if (buffer[i_buff2++] != 'M')
            return false;
        i_buff2++; // step over the ','

        // N field
        while ((i_buff2 < i_buff)&&(buffer[i_buff2]!=','))  //reach the next ','
            i_buff2++;
        if (i_buff2 >= i_buff)
            return false;
        i_buff2++; // step over the ','
        if (buffer[i_buff2++] != 'N')
            return false;
        i_buff2++; // step over the ','
        
        // K field
        bool before_dot = true;
        while ((i_buff2 < i_buff)&&(buffer[i_buff2]!=','))  //reach the next ','
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
                    // convert the ascii digit to speed decimal
                    // un float est trop gourmand... +600 instructions pour
                    // la routine suivante:
                    // read_speed = read_speed * 10 + (buffer[i_buff2]-0x30);
                    // utiliser deux unsigned short pour coder la vitesse!
                    // et faire une structure et des fonctions de calcul
                    if (before_dot)
                        //read_speed = read_speed * 10 + (buffer[i_buff2]-0x30);
                        read_speed_int = read_speed_int*10 + (buffer[i_buff2]-0x30);
                    else
                    {
                        //read_speed = read_speed * 10 + (buffer[i_buff2]-0x30);
                        read_speed_dec = read_speed_int*10 + (buffer[i_buff2]-0x30);
                    }
                    i_buff2++;
                }
            }
        if (i_buff2 >= i_buff)
            return false;
        i_buff2++; // step over the ','
        if (buffer[i_buff2++] != 'M')
            return false;
        i_buff2++; // step over the ','
        
        // now that we have the speed, let's reach the checksum
        while ((i_buff2 < i_buff)&&(buffer[i_buff2]!='*'))
            i_buff2++;
        if (i_buff2 >= i_buff)
            return false;
        unsigned char checksum = 0;
        for (unsigned char i=0;i<i_buff2;i++)
            checksum ^= buffer[i];
        
        // now compare checksum & buffer[i_buff2]/buffer[i_buff2+1]
        // if not equal then stop here!
        
        // update the speed
        speed_int = read_speed_int;
        speed_dec = read_speed_dec;
        return true;
    }
    else return false;
    */