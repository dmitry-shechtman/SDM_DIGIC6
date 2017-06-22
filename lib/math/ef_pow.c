/* ef_pow.c -- float version of e_pow.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */



#include "fdlibm.h"
#include "stdlib.h"

static const float
bp[] = {1.0, 1.5,},
dp_h[] = { 0.0, 5.84960938e-01,}, /* 0x3f15c000 */
dp_l[] = { 0.0, 1.56322085e-06,}, /* 0x35d1cfdc */
zero    =  0.0,
one	=  1.0,
two	=  2.0,
two24	=  16777216.0,	/* 0x4b800000 */
huge	=  1.0e30,
tiny    =  1.0e-30,
	
L1  =  6.0000002384e-01, /* 0x3f19999a */
L2  =  4.2857143283e-01, /* 0x3edb6db7 */
L3  =  3.3333334327e-01, /* 0x3eaaaaab */
L4  =  2.7272811532e-01, /* 0x3e8ba305 */
L5  =  2.3066075146e-01, /* 0x3e6c3255 */
L6  =  2.0697501302e-01, /* 0x3e53f142 */
P1   =  1.6666667163e-01, /* 0x3e2aaaab */
P2   = -2.7777778450e-03, /* 0xbb360b61 */
P3   =  6.6137559770e-05, /* 0x388ab355 */
P4   = -1.6533901999e-06, /* 0xb5ddea0e */
P5   =  4.1381369442e-08, /* 0x3331bb4c */
lg2  =  6.9314718246e-01, /* 0x3f317218 */
lg2_h  =  6.93145752e-01, /* 0x3f317200 */
lg2_l  =  1.42860654e-06, /* 0x35bfbe8c */
ovt =  4.2995665694e-08, 
cp    =  9.6179670095e-01, /* 0x3f76384f =2/(3ln2) */
cp_h  =  9.6179199219e-01, /* 0x3f763800 =head of cp */
cp_l  =  4.7017383622e-06, /* 0x369dc3a0 =tail of cp_h */
ivln2    =  1.4426950216e+00, /* 0x3fb8aa3b =1/ln2 */
ivln2_h  =  1.4426879883e+00, /* 0x3fb8aa00 =16b 1/ln2*/
ivln2_l  =  7.0526075433e-06; /* 0x36eca570 =1/ln2 tail*/

float powf(float x, float y)
{
	float z,ax,z_h,z_l,p_h,p_l;
	float y1,t1,t2,r,s,t,u,v,w;
	int i,j,k,yisint,n;
	int hx,hy,ix,iy,is;

	GET_FLOAT_WORD(hx,x);
	GET_FLOAT_WORD(hy,y);
	ix = hx&0x7fffffff;  iy = hy&0x7fffffff;

    
	if(FLT_UWORD_IS_ZERO(iy)) return one; 	

    /* +-NaN return x+y */
	if(FLT_UWORD_IS_NAN(ix) ||
	   FLT_UWORD_IS_NAN(iy))
		return x+y;	

    
	yisint  = 0;
	if(hx<0) {	
	    if(iy>=0x4b800000) yisint = 2; /* even integer y */
	    else if(iy>=0x3f800000) {
		k = (iy>>23)-0x7f;	   /* exponent */
		j = iy>>(23-k);
		if((j<<(23-k))==iy) yisint = 2-(j&1);
	    }		
	} 

    /* special value of y */
	if (FLT_UWORD_IS_INFINITE(iy)) {	/* y is +-inf */
	    if (ix==0x3f800000)
	        return  y - y;	/* inf**+-1 is NaN */
	    else if (ix > 0x3f800000)
	        return (hy>=0)? y: zero;
	    else			
	        return (hy<0)?-y: zero;
	} 
	if(iy==0x3f800000) {	/* y is  +-1 */
	    if(hy<0) return one/x; else return x;
	}
	if(hy==0x40000000) return x*x; /* y is  2 */
	if(hy==0x3f000000) {	/* y is  0.5 */
	    if(hx>=0)	
	    return sqrtf(x);	
	}

	ax   = fabsf(x);
    /* special value of x */
	if(FLT_UWORD_IS_INFINITE(ix)||FLT_UWORD_IS_ZERO(ix)||ix==0x3f800000){
	    z = ax;			/*x is +-0,+-inf,+-1*/
	    if(hy<0) z = one/z;	
	    if(hx<0) {
		if(((ix-0x3f800000)|yisint)==0) {
		    z = (z-z)/(z-z); 
		} else if(yisint==1) 
		    z = -z;		
	    }
	    return z;
	}
    
    /* (x<0)**(non-int) is NaN */
	if(((((unsigned int)hx>>31)-1)|yisint)==0) return (x-x)/(x-x);

    /* |y| is huge */
	if(iy>0x4d000000) { /* if |y| > 2**27 */
	/* over/underflow if x is not close to one */
	    if(ix<0x3f7ffff8) return (hy<0)? huge*huge:tiny*tiny;
	    if(ix>0x3f800007) return (hy>0)? huge*huge:tiny*tiny;
	
	    t = x-1;		/* t has 20 trailing zeros */
	    w = (t*t)*((float)0.5-t*((float)0.333333333333-t*(float)0.25));
	    u = ivln2_h*t;	/* ivln2_h has 16 sig. bits */
	    v = t*ivln2_l-w*ivln2;
	    t1 = u+v;
	    GET_FLOAT_WORD(is,t1);
	    SET_FLOAT_WORD(t1,is&0xfffff000);
	    t2 = v-(t1-u);
	} else {
	    float s2,s_h,s_l,t_h,t_l;
	    n = 0;
	/* take care subnormal number */
	    if(FLT_UWORD_IS_SUBNORMAL(ix))
		{ax *= two24; n -= 24; GET_FLOAT_WORD(ix,ax); }
	    n  += ((ix)>>23)-0x7f;
	    j  = ix&0x007fffff;
	/* determine interval */
	    ix = j|0x3f800000;		/* normalize ix */
	    if(j<=0x1cc471) k=0;	/* |x|<sqrt(3/2) */
	    else if(j<0x5db3d7) k=1;	/* |x|<sqrt(3)   */
	    else {k=0;n+=1;ix -= 0x00800000;}
	    SET_FLOAT_WORD(ax,ix);

	
	    u = ax-bp[k];		/* bp[0]=1.0, bp[1]=1.5 */
	    v = one/(ax+bp[k]);
	    s = u*v;
	    s_h = s;
	    GET_FLOAT_WORD(is,s_h);
	    SET_FLOAT_WORD(s_h,is&0xfffff000);
	/* t_h=ax+bp[k] High */
	    SET_FLOAT_WORD(t_h,((ix>>1)|0x20000000)+0x0040000+(k<<21));
	    t_l = ax - (t_h-bp[k]);
	    s_l = v*((u-s_h*t_h)-s_h*t_l);
	/* compute log(ax) */
	    s2 = s*s;
	    r = s2*s2*(L1+s2*(L2+s2*(L3+s2*(L4+s2*(L5+s2*L6)))));
	    r += s_l*(s_h+s);
	    s2  = s_h*s_h;
	    t_h = (float)3.0+s2+r;
	    GET_FLOAT_WORD(is,t_h);
	    SET_FLOAT_WORD(t_h,is&0xfffff000);
	    t_l = r-((t_h-(float)3.0)-s2);
	
	    u = s_h*t_h;
	    v = s_l*t_h+t_l*s;
	
	    p_h = u+v;
	    GET_FLOAT_WORD(is,p_h);
	    SET_FLOAT_WORD(p_h,is&0xfffff000);
	    p_l = v-(p_h-u);
	    z_h = cp_h*p_h;		
	    z_l = cp_l*p_h+p_l*cp+dp_l[k];
	
	    t = (float)n;
	    t1 = (((z_h+z_l)+dp_h[k])+t);
	    GET_FLOAT_WORD(is,t1);
	    SET_FLOAT_WORD(t1,is&0xfffff000);
	    t2 = z_l-(((t1-t)-dp_h[k])-z_h);
	}

	s = one; 
	if(((((unsigned int)hx>>31)-1)|(yisint-1))==0)
	    s = -one;	/* (-ve)**(odd int) */

    /* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
	GET_FLOAT_WORD(is,y);
	SET_FLOAT_WORD(y1,is&0xfffff000);
	p_l = (y-y1)*t1+y*t2;
	p_h = y1*t1;
	z = p_l+p_h;
	GET_FLOAT_WORD(j,z);
	i = j&0x7fffffff;
	if (j>0) {
	    if (i>FLT_UWORD_EXP_MAX)
	        return s*huge*huge;			/* overflow */
	    else if (i==FLT_UWORD_EXP_MAX)
	        if(p_l+ovt>z-p_h) return s*huge*huge;	/* overflow */
        } else {
	    if (i>FLT_UWORD_EXP_MIN)
	        return s*tiny*tiny;			/* underflow */
	    else if (i==FLT_UWORD_EXP_MIN)
	        if(p_l<=z-p_h) return s*tiny*tiny;	/* underflow */
	}
    /*
     * compute 2**(p_h+p_l)
     */
	k = (i>>23)-0x7f;
	n = 0;
	if(i>0x3f000000) {		
	    n = j+(0x00800000>>(k+1));
	    k = ((n&0x7fffffff)>>23)-0x7f;	/* new k for n */
	    SET_FLOAT_WORD(t,n&~(0x007fffff>>k));
	    n = ((n&0x007fffff)|0x00800000)>>(23-k);
	    if(j<0) n = -n;
	    p_h -= t;
	} 
	t = p_l+p_h;
	GET_FLOAT_WORD(is,t);
	SET_FLOAT_WORD(t,is&0xfffff000);
	u = t*lg2_h;
	v = (p_l-(t-p_h))*lg2+t*lg2_l;
	z = u+v;
	w = v-(z-u);
	t  = z*z;
	t1  = z - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
	r  = (z*t1)/(t1-two)-(w+z*w);
	z  = one-(r-z);
	GET_FLOAT_WORD(j,z);
	j += (n<<23);
	if((j>>23)<=0) z = scalbnf(z,(int)n);	/* subnormal output */
	else SET_FLOAT_WORD(z,j);
	return s*z;
}
