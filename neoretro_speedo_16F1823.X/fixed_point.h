/* 
 * File:   fixed_point.h
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "mcc_generated_files/mcc.h"

// 24 bits, 10.14
#define FIXED_POINT_TOTAL_SIZE  24
#define FIXED_POINT_FRACT       14
typedef unsigned short long t_fp;

t_fp convert_to_fp(unsigned short int_part, unsigned short fract_part);

t_fp multiply_fp(t_fp n1, t_fp n2);
t_fp  add(t_fp n1, t_fp n2);
t_fp  substract(t_fp n1, t_fp n2);
