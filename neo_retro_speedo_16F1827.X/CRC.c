/* 
 * File:   CRC.c
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "CRC.h"

unsigned short crc_update(unsigned short old_crc, unsigned char data)
{
  unsigned short crc;
  unsigned short x;

  x = ((old_crc>>8) ^ data) & 0xff;
  x ^= x>>4;

  crc = (old_crc << 8) ^ (x << 12) ^ (x <<5) ^ x;

  return crc;
}
