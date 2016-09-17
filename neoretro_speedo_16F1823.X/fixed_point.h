/* 
 * File:   fixed_point.h
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "mcc_generated_files/mcc.h"

// 16 bits, 12.4
#define FIXED_POINT_FRACT   4
typedef unsigned short t_fp;

t_fp convert_to_fp(unsigned short int_part, unsigned short fract_part);

t_fp multiply_fp(t_fp n1, t_fp n2);

//t_fp97  add(t_fp97 n1, t_fp97 n2);
//t_fp97  substract(t_fp97 n1, t_fp97 n2);
//t_fp97  multiply(t_fp97 n1, t_fp97 n2);
//t_fp97  divide(t_fp97 n1, t_fp97 n2);
