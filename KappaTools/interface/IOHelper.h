/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#ifndef TBIOHELPER_H
#define TBIOHELPER_H

#include <iostream>
#include <iomanip>
#include "Artus/KappaTools/interface/VectorTools.h"
#include <set>
#include <map>
#include <boost/io/ios_state.hpp>
#include <boost/property_tree/ptree.hpp>

typedef boost::io::ios_all_saver OStreamGuard;

static const std::string reset = "\33[0m";
static const std::string red = "\33[31m";
static const std::string green = "\33[32m";
static const std::string yellow = "\33[33m";
static const std::string blue = "\33[34m";

template<typename T>
void printMatrix(T **matrix, const size_t x, const size_t y, const size_t width = 10);
void printPTree(const boost::property_tree::ptree &pt, const std::string prefix = "");

namespace __gnu_cxx
{
	template<typename T1, typename T2>
	std::ostream &operator<<(std::ostream &os, const std::map<T1, T2> &m);
	template<typename T>
	std::ostream &operator<<(std::ostream &os, const std::set<T> &s);
	template<typename T>
	std::ostream &operator<<(std::ostream &os, const std::vector<T> &v);
	template<typename T1, typename T2>
	std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p);
}
#include "IOHelper.hxx"

#endif
