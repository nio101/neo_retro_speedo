/* 
 * File:   calibration.h
 * Author: nio
 *
 * Created on 22 septembre 2016, 11:39
 */

#ifndef CALIBRATION_H
#define	CALIBRATION_H

#include "mcc_generated_files/mcc.h"
#include "main.h"

struct conf_data
{
    bool    use_mph;
    uint16  max_pwm;
    uint8   nb_steps;
    uint16  ref_pwm[20];
    uint16  low_speed_pwm;
    uint8   impulse_duration;
} m_conf;

bool load_calibration_from_EEPROM();
void write_calibration_to_EEPROM();
void perform_calibration();

#endif	/* CALIBRATION_H */

