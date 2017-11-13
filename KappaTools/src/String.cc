#include "Artus/KappaTools/interface/String.h"
#include "Artus/KappaTools/interface/Memory.h"
#include <algorithm>
#include <iomanip>
#include <stdlib.h>

using namespace std;

static MemoryPool<string> gStringPool;

_S::_S()
{
	myp = -1;
	mystring = "";
}

_S::_S(const char *s)
{
	myp = -1;
	mystring = string(s);
}

_S::_S(const string &s)
{
	myp = -1;
	mystring = s;
}

_S::operator const char*() const
{
	return gStringPool.AddObject(mystring).c_str();
}

_S::operator const string() const
{
	return mystring;
}

_S &_S::SetPrecision(const int p)
{
	myp = p;
	return *this;
}

_S &_S::operator +(const _S x)
{
	mystring += x.mystring;
	return *this;
}

_S &_S::operator +(const double x)
{
	if (myp >= 0)
		mystring += str(x, myp);
	else
		mystring += KappaTools::str(x);
	return *this;
}

_S &_S::operator +(const bool x)
{
	mystring += x ? "true" : "false";
	return *this;
}

const char *randomstr(const string prefix)
{
	return gStringPool.AddObject(prefix + KappaTools::str(rand())).c_str();
}

string str(const double &i, const int p)
{
	ostringstream str;
	str << setprecision(p) << i;
	return str.str();
}

const char *c_str(const _S input)
{
	return gStringPool.AddObject(input).c_str();
}
