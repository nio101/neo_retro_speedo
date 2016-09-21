/* 
 * File:   main.h
 * Author: nicolas
 *
 * Created on September 20, 2016, 6:59 AM
 */


#define __delay_sec(x) for(unsigned char __delay_tmp=0;__delay_tmp<(10*x);__delay_tmp++){__delay_ms(100);}

#define uint8  unsigned char
#define uint16 unsigned short
#define uint24 unsigned short long
#define uint32 unsigned long

bool load_calibration_from_EEPROM();
void write_calibration_to_EEPROM();
void perform_calibration();
