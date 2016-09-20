/* 
 * File:   CRC.h
 * Author: nio
 *
 * Created on 20 septembre 2016, 10:31
 */

#ifndef CRC_H
#define	CRC_H

#include "mcc_generated_files/mcc.h"

unsigned short crc_update(unsigned short old_crc, unsigned char data);

#endif	/* CRC_H */
