#ifndef TBMATH_H
#define TBMATH_H

#include <string>
#include <algorithm>
#include <cmath>

#ifndef M_PI
static const double M_PI = 3.141592653589793238;
#endif

typedef std::pair<double, double> edouble;

template <typename T>
inline int sgn(const T a)
{
	return (a > T(0)) - (a < T(0));
}

template<typename T>
inline T sqr(const T a)
{
	return a * a;
}

template<typename T>
inline T min(const T a, const T b, const T c)
{
	return std::min(std::min(a, b), c);
}

template<typename T>
inline T max(const T a, const T b, const T c)
{
	return std::max(std::max(a, b), c);
}

double fround_p(double n, unsigned int d);
double fround(double n, int p);
edouble fround(edouble x);
std::pair<std::string, std::string> sround(edouble x);
std::string str(edouble x, int p = -1);

#endif
