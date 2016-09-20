/* 
 * File:   main.h
 * Author: nicolas
 *
 * Created on September 20, 2016, 6:59 AM
 */


#define __delay_sec(x) for(unsigned char tmp=0;tmp<(10*x);tmp++){__delay_ms(100);}

#define uint8  unsigned char
#define uint16 unsigned short
#define uint24 unsigned short long
#define uint32 unsigned long
