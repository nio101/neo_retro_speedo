/* 
 * File:   fixed_point.c
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "fixed_point.h"

t_fp  convert_to_fp(unsigned short int_part, unsigned short fract_part)
{
    // ! int_part & fract_part should be <= 65535 !
    // fract_part should be in 1/10000th (4 digits after dot)
    // example: for 0.621371, fract_part should be 6213
    // example 2: for 0.005, fract_part should be 50
    
    t_fp res = 0;
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

t_fp multiply_fp(t_fp n1, t_fp n2)  //!OVERFLOW POSSIBLE!
{
    t_fp n1_fra, n2_fra;
    n1_fra = (n1 << (FIXED_POINT_TOTAL_SIZE-FIXED_POINT_FRACT)) >> (FIXED_POINT_TOTAL_SIZE-FIXED_POINT_FRACT);
    n2_fra = (n2 << (FIXED_POINT_TOTAL_SIZE-FIXED_POINT_FRACT)) >> (FIXED_POINT_TOTAL_SIZE-FIXED_POINT_FRACT);
    unsigned long long res = ((unsigned long long) (n1 >> FIXED_POINT_FRACT) * (unsigned long long) (n2 >> FIXED_POINT_FRACT))<<FIXED_POINT_FRACT;
    res+ = ((unsigned long long) (n1 >> FIXED_POINT_FRACT) * (unsigned long long) n2_fra);
    res+ = ((unsigned long long) (n2 >> FIXED_POINT_FRACT) * (unsigned long long) n1_fra);
    res+ = ((unsigned long long) n1_fra * (unsigned long long) n2_fra)>>FIXED_POINT_FRACT;
    return res;
}

t_fp  add(t_fp n1, t_fp n2)
{
    return (t_fp)(n1+n2);
}

t_fp  substract(t_fp n1, t_fp n2)
{
    return (t_fp)(n1-n2);
}