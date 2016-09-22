/* 
 * File:   GPS.h
 * Author: nio101
 *
 * Created on September 20, 2016, 6:49 AM
 */


#include "mcc_generated_files/mcc.h"
#include "main.h"
#include "fixed_point.h"

#define NMEA_BUFFER_SIZE 6

void GPS_Initialize(void);
bool GPS_read_speed(void);
