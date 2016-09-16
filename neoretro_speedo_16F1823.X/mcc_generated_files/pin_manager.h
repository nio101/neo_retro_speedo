/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using MPLAB(c) Code Configurator

  @Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  MPLAB(c) Code Configurator - 3.16
        Device            :  PIC16F1823
        Version           :  1.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.20

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

    Microchip licenses to you the right to use, modify, copy and distribute
    Software only when embedded on a Microchip microcontroller or digital signal
    controller that is integrated into your product or third party product
    (pursuant to the sublicense terms in the accompanying license agreement).

    You should refer to the license agreement accompanying this Software for
    additional information regarding your rights and obligations.

    SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
    IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
    CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
    OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
    CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
    SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

*/


#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set BUTTON aliases
#define BUTTON_TRIS               TRISA4
#define BUTTON_LAT                LATA4
#define BUTTON_PORT               RA4
#define BUTTON_WPU                WPUA4
#define BUTTON_ANS                ANSA4
#define BUTTON_SetHigh()    do { LATA4 = 1; } while(0)
#define BUTTON_SetLow()   do { LATA4 = 0; } while(0)
#define BUTTON_Toggle()   do { LATA4 = ~LATA4; } while(0)
#define BUTTON_GetValue()         PORTAbits.RA4
#define BUTTON_SetDigitalInput()    do { TRISA4 = 1; } while(0)
#define BUTTON_SetDigitalOutput()   do { TRISA4 = 0; } while(0)

#define BUTTON_SetPullup()    do { WPUA4 = 1; } while(0)
#define BUTTON_ResetPullup()   do { WPUA4 = 0; } while(0)
#define BUTTON_SetAnalogMode()   do { ANSA4 = 1; } while(0)
#define BUTTON_SetDigitalMode()   do { ANSA4 = 0; } while(0)


// get/set STATUS_LED aliases
#define STATUS_LED_TRIS               TRISA5
#define STATUS_LED_LAT                LATA5
#define STATUS_LED_PORT               RA5
#define STATUS_LED_WPU                WPUA5
#define STATUS_LED_SetHigh()    do { LATA5 = 1; } while(0)
#define STATUS_LED_SetLow()   do { LATA5 = 0; } while(0)
#define STATUS_LED_Toggle()   do { LATA5 = ~LATA5; } while(0)
#define STATUS_LED_GetValue()         PORTAbits.RA5
#define STATUS_LED_SetDigitalInput()    do { TRISA5 = 1; } while(0)
#define STATUS_LED_SetDigitalOutput()   do { TRISA5 = 0; } while(0)

#define STATUS_LED_SetPullup()    do { WPUA5 = 1; } while(0)
#define STATUS_LED_ResetPullup()   do { WPUA5 = 0; } while(0)


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    GPIO and peripheral I/O initialization
 * @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);





#endif // PIN_MANAGER_H
/**
 End of File
*/