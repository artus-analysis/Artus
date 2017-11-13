#include "Artus/KappaTools/interface/Math.h"
#include "Artus/KappaTools/interface/String.h"
#include <cmath>

using namespace std;

double fround_p(double n, unsigned int d)
{
	int p = d - (unsigned int)log10(n);
	return floor(n * pow(10.0, p) + .5) / pow(10.0, p);
}

double fround(double n, int p)
{
	return floor(n * pow(10.0, p) + .5) / pow(10.0, p);
}

edouble fround(edouble x)
{
	double err = fround_p(x.second, 1);
	if (err > 0)
	{
		int p = (int)(log10(err) + 1e-5 - 1);
		return make_pair(fround(x.first, -p), err);	
	}
	else
		return make_pair(x.first, x.second);
}

pair<string, string> sround(edouble x)
{
	double err = fround_p(x.second, 1);
	if (err > 0)
	{
		int p = (int)(log10(err) + 1e-5 - 1);
		int y = (unsigned int)log10(x.first) - p;
		return make_pair(str(fround(x.first, -p), y), str(err, y));	
	}
	else
		return make_pair(KappaTools::str(x.first), KappaTools::str(x.second));	
}

string str(edouble x, int p)
{
	pair<string, string> tmp;
	if (p > 0)
		tmp = make_pair(KappaTools::str(fround_p(x.first, p)), KappaTools::str(fround_p(x.second, p)));
	else
		tmp = sround(x);
	return tmp.first + " +/- " + tmp.second;
}
