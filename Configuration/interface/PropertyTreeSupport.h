/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <cstdint>

#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "Artus/Utility/interface/Collections.h"

/*
 * This class provides various convenience functions when working with Boost PropertyTrees.
 */
class PropertyTreeSupport {
public:
	template<class TSetting>
	static std::vector< TSetting > GetAsList(boost::property_tree::ptree * propTree, std::string path)
	{
		std::vector< TSetting > vec;
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v, propTree->get_child(path))
		{
			vec.push_back(boost::lexical_cast<TSetting>(v.second.data().c_str()));
		}
		return vec;
	}

};
