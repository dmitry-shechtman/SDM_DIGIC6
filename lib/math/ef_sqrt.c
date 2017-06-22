/* ef_sqrtf.c -- float version of e_sqrt.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */



#include "fdlibm.h"

#ifdef __STDC__
static	const float	one	= 1.0, tiny=1.0e-30;
#else
static	float	one	= 1.0, tiny=1.0e-30;
#endif

float sqrtf(float x)
{
	float z;
	int 	sign = (int)0x80000000; 
	unsigned int r,hx;
	int ix,s,q,m,t,i;

	GET_FLOAT_WORD(ix,x);
	hx = ix&0x7fffffff;

    /* take care of Inf and NaN */
	if(!FLT_UWORD_IS_FINITE(hx))
	    return x*x+x;		
    /* take care of zero and -ves */
	if(FLT_UWORD_IS_ZERO(hx)) return x;
	if(ix<0) return (x-x)/(x-x);		

    /* normalize x */
	m = (ix>>23);
	if(FLT_UWORD_IS_SUBNORMAL(hx)) {		/* subnormal x */
	    for(i=0;(ix&0x00800000L)==0;i++) ix<<=1;
	    m -= i-1;
	}
	m -= 127;	/* unbias exponent */
	ix = (ix&0x007fffffL)|0x00800000L;
	if(m&1)	/* odd m, double x to make it even */
	    ix += ix;
	m >>= 1;	

    /* generate sqrt(x) bit by bit */
	ix += ix;
	q = s = 0;		
	r = 0x01000000L;		

	while(r!=0) {
	    t = s+r; 
	    if(t<=ix) { 
		s    = t+r; 
		ix  -= t; 
		q   += r; 
	    } 
	    ix += ix;
	    r>>=1;
	}

    /* use floating add to find out rounding direction */
	if(ix!=0) {
	    z = one-tiny; /* trigger inexact flag */
	    if (z>=one) {
	        z = one+tiny;
		if (z>one)
		    q += 2;
		else
		    q += (q&1);
	    }
	}
	ix = (q>>1)+0x3f000000L;
	ix += (m <<23);
	SET_FLOAT_WORD(z,ix);
	return z;
}
