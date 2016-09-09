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

/*
 * Helpers
 */

#define __delay_sec(x) for(unsigned char tmp=0;tmp<(10*x);tmp++){__delay_ms(100);}

unsigned char char_buffer[1];

void UART_Initialize(void)
{
    // VTG every fix, GGA every 5 fix
    //printf("$PMTK220,200*2C\r\n");
    //printf("$PMTK314,0,0,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n");
    //printf("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n");
    //printf("$PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n");
    //printf("$PMTK104*37\r\n");
    __delay_sec(1);
    printf("$PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
    __delay_ms(100);
    printf("$PMTK220,100*2F\r\n");
    __delay_ms(100);
}

void UART_read_NMEA(void) // read & parse/check one NMEA sentence
{   
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
    // et du coup on a une r�solution PWM de 10bits
    // + datasheet p196 pour proc�dure init propre du PWM
    
    // pour l'UART: 9600 baud, 8data bits, no parity
    // 1 stop bit, receive polarity: idle1
    // output: normal (GND/3.3V)
    
    // EEPROM
    
    // pushbutton + debounce timer
    
    // NMEA: $VTG,,,,,*[checksum]<CR><LF>
    // CR:0x0D LF:0x0A
    // max length: 79 chars between '$' and "<CR><LF>"
    // Utiliser $ pour d�tecter le d�but de trame, commencer r�ception
    // si $ => retour d�but de trame/r�init r�ception
    // si <LF> => fin r�ception donc analyse commande, checksum,
    // et lecture param�tres
    // Si buffer d�borde => ignore buffer & stop r�ception.
    
    // printf de configuration du GPS:
    
    // API_SET_NMEA_OUTPUT pour filtrer les messages:
    // $PMTK314,0,0,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2C<CR><LF>
    // => VTG every fix, GGA every 5 fixes, nothing else
    // $PMTK314,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29<CR><LF>
    // => VTG every fix, nothing else)
    
    // SET UPDATE FREQ
    // $PMTK220,100*2F<CR><LF>
    // => position fix done 10 times per second (instead of 1 per second)

    // 1�re �tape: laisser faire l'UART initialize avec les printf
    // et mettre le bus pirate en copie de ce que re�oit le pic
    // => tester reset, gain de fix & perte de fix
    
    // 2�me �tape: lire les trames NMEA, �ventuellement updater la vitesse
    // et la passer � zero si pas de trame r�cente (pour �viter de rester
    // scotch� en cas de perte de fix)
    
    UART_Initialize();

    while (1)
    {
          UART_read_NMEA(); // read/check one NMEA sentence
          // check if last VTG data is old (> how many sec?)
          // if so, we may have lost the fix => PWM to zero?
    }

    /*while (1)
    {
        // Add your application code
        STATUS_LED_SetHigh();
        __delay_sec(1);
        STATUS_LED_SetLow();
        __delay_sec(1);
    }*/
}
/**
 End of File
*/