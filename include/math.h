#ifndef MATH_H
#define MATH_H

#ifndef __VFP_FP__
extern double d2d(double value);
#else
#define d2d(v) (v)
#endif

extern double log(double x);
extern double log2(double x);
extern double log10(double x);
extern double pow(double x, double y);
extern double sqrt(double x);
extern double atan(double x); //defined in math.c

#endif
