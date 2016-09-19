/* 
 * File:   fixed_point.c
 * Author: nio101
 *
 * Created on September 17, 2016, 6:04 PM
 */


#include "fixed_point.h"

// warning: unsigned values only!

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
    t_fp n1_fra, n2_fra;
    n1_fra = n1 & 0x0000FFFF;
    n2_fra = n2 & 0x0000FFFF;
    signed long long res = ((signed long long) (n1 >> FIXED_POINT_FRACT) * (signed long long) (n2 >> FIXED_POINT_FRACT))<<FIXED_POINT_FRACT;
    res+ = ((signed long long) (n1 >> FIXED_POINT_FRACT) * (signed long long) n2_fra);
    res+ = ((signed long long) (n2 >> FIXED_POINT_FRACT) * (signed long long) n1_fra);
    res+ = ((signed long long) n1_fra * (signed long long) n2_fra)>>FIXED_POINT_FRACT;
    return res;
}

t_fp reciprocal_fp(t_fp n)  // using Newton?Raphson division
{
    //t_fp a = fp_x0_a;
    //t_fp b = multiply_fp(fp_x0_b, n);
    //t_fp X0 = a - b;
    
    // Apply a bit-shift to the divisor D to scale it so that 0.5 ? D ? 1.
    // => le faire sur n en notant le nombre de bitshifts vers la droite
    // il faudra faire l'inverse sur le résultat trouvé à la fin!
    
    // uniquement si 0.5 ? n ? 1
    t_fp x = fp_x0_a - multiply_fp(fp_x0_b, n);
    
    // begin iterations
    // automatiser avec un delta threshold qui arrête la boucle
    // + nombre max si delta non atteint
    for (unsigned char i=0; i<=10; i++)
        x = x + multiply_fp(x, (0x00010000 - multiply_fp(x, n)));
    return x;
}

/*t_fp add(t_fp n1, t_fp n2)  //!OVERFLOW POSSIBLE!
{
    return (t_fp)(n1+n2);
}

t_fp substract(t_fp n1, t_fp n2)
{
    return (t_fp)(n1-n2);
}*/
