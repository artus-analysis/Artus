
#include <assert.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Utility/interface/ArtusLogging.h"


stringvector PropertyTreeSupport::GetAsStringList(boost::property_tree::ptree * propTree,
                                                  std::string path, bool failIfNotFound)
{
	stringvector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree->get_child(path))
		{
			vec.push_back(v.second.data());
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional ?
		if (failIfNotFound)
			LOG(FATAL) << "String list on path " << path << " can not be found in configuration file!";
	}
	return vec;
}

doublevector PropertyTreeSupport::GetAsDoubleList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	doublevector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree->get_child(path))
		{
			vec.push_back(boost::lexical_cast<double>(v.second.data().c_str()));
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional
	}
	return vec;
}

doublevector PropertyTreeSupport::GetAsDoubleList(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	doublevector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree.get_child(path))
		{
			vec.push_back(boost::lexical_cast<double>(v.second.data().c_str()));
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional
	}
	return vec;
}

floatvector PropertyTreeSupport::GetAsFloatList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	floatvector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree->get_child(path))
		{
			vec.push_back(boost::lexical_cast<float>(v.second.data().c_str()));
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional
	}
	return vec;
}

floatvector PropertyTreeSupport::GetAsFloatList(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	floatvector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree.get_child(path))
		{
			vec.push_back(boost::lexical_cast<float>(v.second.data().c_str()));
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional
	}
	return vec;
}

intvector PropertyTreeSupport::GetAsIntList(boost::property_tree::ptree & propTree,
                                            std::string path)
{
	intvector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree.get_child(path))
		{
			vec.push_back(boost::lexical_cast<int>(v.second.data().c_str()));
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional
	}
	return vec;
}

intvector PropertyTreeSupport::GetAsIntList(boost::property_tree::ptree * propTree,
                                            std::string path)
{
	intvector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree->get_child(path))
		{
			vec.push_back(boost::lexical_cast<int>(v.second.data().c_str()));
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional
	}
	return vec;
}

uint64vector PropertyTreeSupport::GetAsUInt64List(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	uint64vector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree.get_child(path))
		{
			vec.push_back(boost::lexical_cast<uint64_t>(v.second.data().c_str()));
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional
	}
	return vec;
}

uint64vector PropertyTreeSupport::GetAsUInt64List(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	uint64vector vec;
	try {
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				propTree->get_child(path))
		{
			vec.push_back(boost::lexical_cast<uint64_t>(v.second.data().c_str()));
		}
	} catch (boost::property_tree::ptree_bad_path& e) {
		// no problem, node optional
	}
	return vec;
}
