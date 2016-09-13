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
/*
 * Helpers
 */

void my10msTimerISR(void);  // custom function called every 10ms,
                            // used for LED blinking and push button debounce

unsigned char volatile LED_counter=0;
LED_state_t LED_state;

#define __delay_sec(x) for(unsigned char tmp=0;tmp<(10*x);tmp++){__delay_ms(100);}

// to store current speed value and units
typedef enum _speed_unit_type { MPH = 0, KPH } speed_unit_type;
speed_unit_type speed_units = MPH;  // MPH by default
float speed = 0.0;

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

void GPS_read_speed(void) // read & parse/check one NMEA sentence
{   
    unsigned char char_buffer[40];
    // we are going to wait for a '$' to begin recording received chars
    // until another '$' comes (restart recording), or an <LF> (parse sentence)
    // if the sentence is a correct VTG, read the speed and reset dedicated tmr
    bool done = false;
    unsigned char m_char;
    while (!done)
    {
        if(eusartRxCount==0)
        {
            STATUS_LED_SetLow();
        }        
        else if(eusartRxCount!=0)
        {
            m_char=EUSART_Read();  // read a byte for RX
            STATUS_LED_SetHigh();
            __delay_ms(1);
        }
    }
}

/*
                         Main application
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    TMR0_SetInterruptHandler(my10msTimerISR);
    
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
    
    // Test DA LED output! :)
    LED_set_state(always_on);
    __delay_sec(5);
    LED_set_state(always_off);
    __delay_sec(5);
    LED_set_state(slow_blinking);
    __delay_sec(5);
    LED_set_state(fast_blinking);
    __delay_sec(5);
    
    while(1)
    {}
    
    // test PWM now!
    
    //uint16_t motor_load = 128; // == 0-1024 for 0-100% motor load
    uint16_t motor_load = 0;
    EPWM_LoadDutyValue(1023);    
    TMR2_StartTimer();
    __delay_sec(5);
    
    GPS_Initialize();
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

    /*while (1)
    {
        // Add your application code
        STATUS_LED_SetHigh();
        __delay_sec(1);
        STATUS_LED_SetLow();
        __delay_sec(1);
    }*/
}

void my10msTimerISR(void)
{
    LED_update_loop();
}

/**
 End of File
*/