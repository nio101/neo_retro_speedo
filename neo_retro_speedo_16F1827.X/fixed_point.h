/* 
 * File:   fixed_point.h
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "mcc_generated_files/mcc.h"

// 32 bits signed integer used for unsigned FP16.16 math

#define FIXED_POINT_TOTAL_SIZE  32
#define FIXED_POINT_FRACT       16
typedef signed long long t_fp;    // 32 bits signed integer

t_fp convert_to_fp(signed short int_part, signed short fract_part);
t_fp multiply_fp(t_fp n1, t_fp n2);
t_fp reciprocal_fp(t_fp n);

// note that addition and substraction are directly done
// using +/- with t_fp
