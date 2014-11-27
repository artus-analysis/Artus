/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <cstdint>

#include <boost/property_tree/ptree.hpp>

#include "Artus/Utility/interface/Collections.h"

/*
 * This class provides various convinience functions when working with Boost PropertyTrees.
 */
class PropertyTreeSupport {
public:
	static stringvector GetAsStringList(boost::property_tree::ptree * propTree,
	                                    std::string path, bool failIfNotFound = false);

	static doublevector GetAsDoubleList(boost::property_tree::ptree * propTree,
	                                    std::string path);

	static doublevector GetAsDoubleList(boost::property_tree::ptree & propTree,
	                                    std::string path);

	static floatvector GetAsFloatList(boost::property_tree::ptree * propTree,
	                                  std::string path);

	static floatvector GetAsFloatList(boost::property_tree::ptree & propTree,
	                                  std::string path);

	static intvector GetAsIntList(boost::property_tree::ptree & propTree,
	                              std::string path);

	static intvector GetAsIntList(boost::property_tree::ptree * propTree,
	                              std::string path);

	static uint64vector GetAsUInt64List(boost::property_tree::ptree & propTree,
	                                    std::string path);

	static uint64vector GetAsUInt64List(boost::property_tree::ptree * propTree,
	                                    std::string path);
};
