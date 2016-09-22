/* 
 * File:   fixed_point.c
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */

#include "fixed_point.h"

t_fp  convert_to_fp(signed short int_part, signed short fract_part)
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
    if (res < 0)
        res = res - res_fract;
    else
        res = res + res_fract;
    return res;
}

t_fp multiply_fp(t_fp n1, t_fp n2)  //!OVERFLOW POSSIBLE!
{
    // decompose n1 & n2 in integer + fractional parts & compute them
    // to minimize the resolution loss
    signed long long res = ((signed long long) (n1 >> FIXED_POINT_FRACT) * (signed long long) (n2 >> FIXED_POINT_FRACT))<<FIXED_POINT_FRACT;
    res+= ((signed long long) (n1 >> FIXED_POINT_FRACT) * (signed long long) (n2 & 0x0000FFFF));
    res+= ((signed long long) (n2 >> FIXED_POINT_FRACT) * (signed long long) (n1 & 0x0000FFFF));
    res+= (((signed long long) (n1 & 0x0000FFFF) * (signed long long) (n2 & 0x0000FFFF))>>FIXED_POINT_FRACT)& 0x0000FFFF;
    return res;
}

t_fp reciprocal_fp(t_fp n)  // using Newton Raphson algorithm
{
    // Apply a bit-shift to the divisor D to scale it so that 0.5 <= D <= 1.
    signed char shift = 0;
    bool neg = (n < 0);
    if (neg)
        n = -n;
    if (n < 0x00008000) // 0.5
        while (n < 0x00008000) // 0.5
        {
            n = n << 1;
            shift ++;
        }
    else if (n > 0x00010000) // 1
        while (n > 0x00010000) // 1
        {
            n = n >> 1;
            shift --;
        }
    t_fp x = fp_x0_a - multiply_fp(fp_x0_b, n);
    // begin iterations - 3 are enough
    for (unsigned char i=0; i<3; i++)
        x = multiply_fp(x, (0x00020000 - multiply_fp(x, n)));
        //x = x + multiply_fp(x, (0x00010000 - multiply_fp(x, n))); // same
    // now we have to shift also the result
    if (shift < 0)
        x = x >> (-shift);
    else if (shift > 0)
        x = x << (shift);
    if (neg)
        return -x;
    else
        return x;
}

signed short integer_part(t_fp n)
{
    bool neg = (n<0);
    if (neg)
        n = -n;
    unsigned short int_part = n >> 16;
    t_fp and_a_half = convert_to_fp(int_part, 5000);
    if (n > and_a_half)
        int_part++;
    if (neg)
        return -int_part;
    else
        return int_part;
}
