/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/foreach.hpp>
#include "Artus/Core/interface/GlobalInclude.h"

/*
 * This class provides various convinience functions when working with Boost PropertyTrees.
 */
class PropertyTreeSupport {
public:
	static stringvector GetAsStringList(boost::property_tree::ptree * propTree,
			std::string path, bool failIfNotFound = false) {
		stringvector fvec;
		try {
			BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
					propTree->get_child(path))
			{
				fvec.push_back(v.second.data());
			}
		} catch (boost::property_tree::ptree_bad_path& e) {
			// no problem, node optional ?
			if (failIfNotFound)
				CALIB_LOG_FATAL(
						"String list on path " + path
								+ " can not be found in configuration file")
		}
		return fvec;
	}

	static doublevector GetAsDoubleList(boost::property_tree::ptree * propTree,
			std::string path) {
		doublevector fvec;
		try {
			BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
					propTree->get_child(path))
			{
				fvec.push_back(atof(v.second.data().c_str()));
			}
		} catch (boost::property_tree::ptree_bad_path& e) {
			// no problem, node optional
		}
		return fvec;
	}

	static doublevector GetAsDoubleList(boost::property_tree::ptree & propTree,
			std::string path) {
		doublevector fvec;
		try {
			BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
					propTree.get_child(path))
			{
				fvec.push_back(atof(v.second.data().c_str()));
			}
		} catch (boost::property_tree::ptree_bad_path& e) {
			// no problem, node optional
		}
		return fvec;
	}

	static intvector GetAsIntList(boost::property_tree::ptree & propTree,
			std::string path) {
		intvector fvec;
		try {
			BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
					propTree.get_child(path))
			{
				fvec.push_back(atof(v.second.data().c_str()));
			}
		} catch (boost::property_tree::ptree_bad_path& e) {
			// no problem, node optional
		}
		return fvec;
	}
	static intvector GetAsIntList(boost::property_tree::ptree * propTree,
			std::string path) {
		intvector fvec;
		try {
			BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
					propTree->get_child(path))
			{
				fvec.push_back(atof(v.second.data().c_str()));
			}
		} catch (boost::property_tree::ptree_bad_path& e) {
			// no problem, node optional
		}
		return fvec;
	}
};
