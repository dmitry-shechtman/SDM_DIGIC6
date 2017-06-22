/* sf_fabs.c -- float version of s_fabs.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */



/*
 * fabsf(x) returns the absolute value of x.
 */

#include "fdlibm.h"

float fabsf(float x)
{
	unsigned int ix;
	GET_FLOAT_WORD(ix,x);
	SET_FLOAT_WORD(x,ix&0x7fffffff);
        return x;
}

#ifdef _DOUBLE_IS_32BITS

double fabs(double x)
{
	return (double) fabsf((float) x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
