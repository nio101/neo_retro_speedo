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

const t_fp fp_x0_a = 0x0002D2D0;  // 2.8235
const t_fp fp_x0_b = 0x0001E1E4;  // 1.8824

t_fp convert_to_fp(signed short int_part, signed short fract_part);
t_fp multiply_fp(t_fp n1, t_fp n2);
t_fp reciprocal_fp(t_fp n);

// note that addition and substraction are directly done
// using +/- with t_fp
