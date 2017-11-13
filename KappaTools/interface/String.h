#ifndef TBSTRING_H
#define TBSTRING_H

#include "Artus/KappaTools/interface/Vector.h"
#include "Artus/KappaTools/interface/StringTools.h"

class _S
{
public:
	_S();
	_S(const char *s);
	_S(const std::string &s);
	operator const char*() const;
	operator const std::string() const;
	_S &SetPrecision(const int p);

	template<typename T>
	_S &operator +(const T x)
	{
		mystring += KappaTools::str<T>(x);
		return *this;
	}
	_S &operator +(const _S x);
	_S &operator +(const double x);
	_S &operator +(const bool x);

	template<typename T>
	_S &operator <<(const T x)
	{
		return this->operator+(x);
	}
private:
	std::string mystring;
	int myp;
};

const char *randomstr(const std::string prefix = "");
std::string str(const double &i, const int p);
const char *c_str(const _S input);

#include "String.hxx"

#endif
