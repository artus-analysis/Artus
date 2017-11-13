/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#include "Artus/KappaTools/interface/VectorTools.h"

using namespace std;

std::vector<int> seq(int last)
{
	std::vector<int> result;
	for (int i = 0; i <= last; ++i)
		result.push_back(i);
	return result;
}

std::vector<int> seq(int start, int last)
{
	std::vector<int> result;
	for (int i = start; i <= last; ++i)
		result.push_back(i);
	return result;
}

std::vector<std::string> match(const std::string &pattern, const std::vector<std::string> &in,
	const boost::regex::flag_type flags)
{
	std::vector<std::string> result;
	boost::regex regexObj(pattern, flags);
	for (std::vector<std::string>::const_iterator it = in.begin(); it != in.end(); ++it)
		if (boost::regex_search(*it, regexObj))
			result.push_back(*it);
	return result;
}
