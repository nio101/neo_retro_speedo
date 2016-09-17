/* 
 * File:   fixed_point.c
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "fixed_point.h"

t_fp  convert_to_fp(unsigned short int_part, unsigned short fract_part)
{
    t_fp res = 0;
    // ! int_part must be 10 bit long at max: !
    if (int_part > 16383)
        return res; // error, return zero
    // fill in the integer part
    res = int_part;
    res = res << FIXED_POINT_FRACT;
    // compute the fractional part
    unsigned short tmp_left = fract_part;
    unsigned short res_fract = 0;
    const unsigned short one = 1;
    for (unsigned char i_bit = 0; i_bit < FIXED_POINT_FRACT; i_bit++)
    {
        tmp_left = tmp_left << 1;
        if (tmp_left >= 10000)
        {
            res_fract = res_fract | one<<(FIXED_POINT_FRACT-i_bit-1);
            tmp_left = tmp_left - 10000;
        }
    }
    // add the fractionnal part
    res = res + res_fract;
    return res;
}

t_fp multiply_fp(t_fp n1, t_fp n2)  //!OVERFLOW POSSIBLE! 24*24 needs 48, not 32
{
    unsigned long long tmp = (unsigned long long) n1 * (unsigned long long) n2;
    t_fp res = tmp >> FIXED_POINT_FRACT;    // !OVERFLOW POSSIBLE! 
    return res;
}