/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef TBVECTORTOOLS_H
#define TBVECTORTOOLS_H

#include <string>
#include <iostream>
#include <iterator>
#include <vector>
#include <boost/algorithm/string/regex.hpp>
#include "Artus/KappaTools/interface/StringTools.h"

std::vector<int> seq(int last);
std::vector<int> seq(int start, int last);
template<typename T>
std::vector<T> seq(T start, T inc, T last);

template<typename TK, typename TV>
std::map<TV, TK> invertMap(const std::map<TK, TV> &in);

template<typename Tlambda, typename Tin>
std::vector<std::string> strmap(const Tlambda &lambda, const Tin &list);

template<typename Tin>
std::vector<std::string> fmtmap(const std::string &str, const Tin &list, const std::string &param = "$1");

template<typename Tlambda, typename Tin>
Tin mymap(const Tlambda &lambda, const Tin &list);

template<typename Tin, typename Tp1>
std::vector<Tp1> getFirst(const Tin &in);
template<typename Tin, typename Tp1>
std::vector<Tp1> getSecond(const Tin &in);

template<typename Tin, typename Tout>
Tout convert(const Tin &in);

template<typename Tk, typename Tv>
std::vector<Tv> match(const std::string &pattern, const std::map<Tk, Tv> &in,
	const boost::regex::flag_type flags = boost::regex::extended);
template<typename Tk, typename Tv>
std::vector<Tk> match_keys(const std::string &pattern, const std::map<Tk, Tv> &in,
	const boost::regex::flag_type flags = boost::regex::extended);
std::vector<std::string> match(const std::string &pattern, const std::vector<std::string> &in,
	const boost::regex::flag_type flags = boost::regex::extended);

template<typename T>
void multiResize(std::vector<T> &v, size_t s1)
{
	v.resize(s1);
}

template<typename T>
void multiResize(std::vector<std::vector<T> > &v, size_t s1, size_t s2)
{
	v.resize(s1);
	for (size_t i1 = 0; i1 < s1; ++i1)
		v[i1].resize(s2);
}

template<typename T>
void multiResize(std::vector<std::vector<T> > &v, size_t s1, size_t s2, size_t s3)
{
	v.resize(s1);
	for (size_t i1 = 0; i1 < s1; ++i1)
	{
		v[i1].resize(s2);
		for (size_t i2 = 0; i2 < s2; ++i2)
			v[i1][i2].resize(s3);
	}
}

#include "VectorTools.hxx"

#endif
