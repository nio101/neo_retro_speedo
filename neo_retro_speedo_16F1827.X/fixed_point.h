/* 
 * File:   fixed_point.h
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "mcc_generated_files/mcc.h"

// warning: unsigned values only!

// 32 bits, 16.16
#define FIXED_POINT_TOTAL_SIZE  16
#define FIXED_POINT_FRACT       16
typedef unsigned long long t_fp;    // 32 bits

t_fp convert_to_fp(unsigned short int_part, unsigned short fract_part);

t_fp multiply_fp(t_fp n1, t_fp n2); //!OVERFLOW POSSIBLE!
t_fp add(t_fp n1, t_fp n2);         //!OVERFLOW POSSIBLE!
t_fp substract(t_fp n1, t_fp n2);
