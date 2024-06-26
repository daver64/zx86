/*

   Copyright 2009 Pierre KRIEGER

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#ifndef _STDC_MATH_H_
#define _STDC_MATH_H_

#ifdef __cplusplus
extern "C"
{
#endif

    // TODO: 7.12.1 and 7.12.2

#ifdef __GNUC__
#define fpclassify(x) __builtin_fpclassify(x)
#define isfinite(x) __builtin_isfinite(x)
#define isinf(x) __builtin_isinf(x)
#define isnan(x) __builtin_isnan(x)
#define isnormal(x) __builtin_isnormal(x)
#define signbit(x) (isnan(x) ? 0 : ((x) < 0))
#else
// note: still to be tested
#define isinf(x) (((x) != 0) && (2 * (x) == (x)))
#define isnan(x) ((x) != (x))
#define isfinite(x) (!isinf(x))
#define isnormal(x) (!isnan(x) && isfinite(x) && ((x) != 0))
#define signbit(x) (isnan(x) ? 0 : ((x) < 0))
#define fpclassify(x) (((x) == 0) ? 2 : (isinf(x) ? 3 : (isnan(x) ? 4 : 1)))
#endif

#define M_PI (3.14159265358979323846264338327950288)
#define MAX_FP_DIGITS (112)
#define NAN __builtin_nan("")
#define INFINITY __builtin_inf()
    // double			atof(char *str);
    double acos(double x);
    float acosf(float x);
    long double acosl(long double x);
    double asin(double x);
    float asinf(float x);
    long double asinl(long double x);
    double atan(double x);
    float atanf(float x);
    long double atanl(long double x);
    double atan2(double y, double x);
    float atan2f(float y, float x);
    long double atan2l(long double y, long double x);
    double cos(double x);
    float cosf(float x);
    long double cosl(long double x);
    double sin(double x);
    float sinf(float x);
    long double sinl(long double x);
    double tan(double x);
    float tanf(float x);
    long double tanl(long double x);
    double acosh(double x);
    float acoshf(float x);
    long double acoshl(long double x);
    double asinh(double x);
    float asinhf(float x);
    long double asinhl(long double x);
    double atanh(double x);
    float atanhf(float x);
    long double atanhl(long double x);
    double cosh(double x);
    float coshf(float x);
    long double coshl(long double x);
    double sinh(double x);
    float sinhf(float x);
    long double sinhl(long double x);
    double tanh(double x);
    float tanhf(float x);
    long double tanhl(long double x);
    double exp(double x);
    float expf(float x);
    long double expl(long double x);
    double exp2(double x);
    float exp2f(float x);
    long double exp2l(long double x);
    double expm1(double x);
    float expm1f(float x);
    long double expm1l(long double x);
    double frexp(double x, int *exp);
    float frexpf(float x, int *exp);
    long double frexpl(long double x, int *exp);
    int ilogb(double x);
    int ilogbf(float x);
    int ilogbl(long double x);
    double ldexp(double x, int exp);
    float ldexpf(float x, int exp);
    long double ldexpl(long double x, int exp);
    double log(double x);
    float logf(float x);
    long double logl(long double x);
    double log10(double x);
    float log10f(float x);
    long double log10l(long double x);
    double log1p(double x);
    float log1pf(float x);
    long double log1pl(long double x);
    double log2(double x);
    float log2f(float x);
    long double log2l(long double x);
    double logb(double x);
    float logbf(float x);
    long double logbl(long double x);
    double modf(double value, double *iptr);
    float modff(float value, float *iptr);
    long double modfl(long double value, long double *iptr);
    double scalbn(double x, int n);
    float scalbnf(float x, int n);
    long double scalbnl(long double x, int n);
    double scalbln(double x, long int n);
    float scalblnf(float x, long int n);
    long double scalblnl(long double x, long int n);
    double cbrt(double x);
    float cbrtf(float x);
    long double cbrtl(long double x);
    double fabs(double x);
    float fabsf(float x);
    long double fabsl(long double x);
    double hypot(double x, double y);
    float hypotf(float x, float y);
    long double hypotl(long double x, long double y);
    double pow(double x, double y);
    float powf(float x, float y);
    long double powl(long double x, long double y);
    double sqrt(double x);
    float sqrtf(float x);
    long double sqrtl(long double x);
    double erf(double x);
    float erff(float x);
    long double erfl(long double x);
    double erfc(double x);
    float erfcf(float x);
    long double erfcl(long double x);
    double lgamma(double x);
    float lgammaf(float x);
    long double lgammal(long double x);
    double tgamma(double x);
    float tgammaf(float x);
    long double tgammal(long double x);
    double ceil(double x);
    float ceilf(float x);
    long double ceill(long double x);
    double floor(double x);
    float floorf(float x);
    long double floorl(long double x);
    double nearbyint(double x);
    float nearbyintf(float x);
    long double nearbyintl(long double x);
    double rint(double x);
    float rintf(float x);
    long double rintl(long double x);
    long int lrint(double x);
    long int lrintf(float x);
    long int lrintl(long double x);
    long long int llrint(double x);
    long long int llrintf(float x);
    long long int llrintl(long double x);
    double round(double x);
    float roundf(float x);
    long double roundl(long double x);
    long int lround(double x);
    long int lroundf(float x);
    long int lroundl(long double x);
    long long int llround(double x);
    long long int llroundf(float x);
    long long int llroundl(long double x);
    double trunc(double x);
    float truncf(float x);
    long double truncl(long double x);
    double fmod(double x, double y);
    float fmodf(float x, float y);
    long double fmodl(long double x, long double y);
    double remainer(double x, double y);
    float remainerf(float x, float y);
    long double remainerl(long double x, long double y);
    double remquo(double x, double y, int *quo);
    float remquof(float x, float y, int *quo);
    long double remquol(long double x, long double y, int *quo);
    double copysign(double x, double y);
    float copysignf(float x, float y);
    long double copysignl(long double x, long double y);
    double nan(const char *tagp);
    float nanf(const char *tagp);
    long double nanl(const char *tagp);
    double nextafter(double x, double y);
    float nextafterf(float x, float y);
    long double nextafterl(long double x, long double y);
    double nexttoward(double x, long double y);
    float nexttowardf(float x, long double y);
    long double nexttowardl(long double x, long double y);
    double fdim(double x, double y);
    float fdimf(float x, float y);
    long double fdiml(long double x, long double y);
    double fmax(double x, double y);
    float fmaxf(float x, float y);
    long double fmaxl(long double x, long double y);
    double fmin(double x, double y);
    float fminf(float x, float y);
    long double fminl(long double x, long double y);
    double fma(double x, double y, double z);
    float fmaf(float x, float y, float z);
    long double fmal(long double x, long double y, long double z);

#ifdef __GNUC__
#define isgreater(x, y) __builtin_isgreater(x, y)
#define isgreaterequal(x, y) __builtin_isgreaterequal(x, y)
#define isless(x, y) __builtin_isless(x, y)
#define islessequal(x, y) __builtin_islessequal(x, y)
#define islessgreater(x, y) __builtin_islessgreater(x, y)
#define isunordered(x, y) __builtin_isunordered(x, y)
#else
#define isgreater(x, y) (isnan(x) ? y : (isnan(y) ? x : ((x) > (y))))
#define isgreaterequal(x, y) (isnan(x) ? y : (isnan(y) ? x : ((x) >= (y))))
#define isless(x, y) (isnan(x) ? y : (isnan(y) ? x : ((x) < (y))))
#define islessequal(x, y) (isnan(x) ? y : (isnan(y) ? x : ((x) <= (y))))
#define islessgreater(x, y) (isnan(x) ? y : (isnan(y) ? x : ((x) < (y) || (x) > (y))))
#define isunordered(x, y) ((isnan(x) || isnan(y)) ? 1 : 0)
#endif

#ifdef __cplusplus
}
#endif

#endif
