
#include <assert.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Utility/interface/ArtusLogging.h"


std::vector<std::string> PropertyTreeSupport::GetAsStringList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	std::vector<std::string> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(v.second.data());
	}
	return vec;
}

std::vector<double> PropertyTreeSupport::GetAsDoubleList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	std::vector<double> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(boost::lexical_cast<double>(v.second.data().c_str()));
	}
	return vec;
}

std::vector<double> PropertyTreeSupport::GetAsDoubleList(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	std::vector<double> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree.get_child(path))
	{
		vec.push_back(boost::lexical_cast<double>(v.second.data().c_str()));
	}
	return vec;
}

std::vector<float> PropertyTreeSupport::GetAsFloatList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	std::vector<float> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(boost::lexical_cast<float>(v.second.data().c_str()));
	}
	return vec;
}

std::vector<float> PropertyTreeSupport::GetAsFloatList(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	std::vector<float> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree.get_child(path))
	{
		vec.push_back(boost::lexical_cast<float>(v.second.data().c_str()));
	}
	return vec;
}

std::vector<int> PropertyTreeSupport::GetAsIntList(boost::property_tree::ptree & propTree,
                                            std::string path)
{
	std::vector<int> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree.get_child(path))
	{
		vec.push_back(boost::lexical_cast<int>(v.second.data().c_str()));
	}
	return vec;
}

std::vector<int> PropertyTreeSupport::GetAsIntList(boost::property_tree::ptree * propTree,
                                            std::string path)
{
	std::vector<int> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(boost::lexical_cast<int>(v.second.data().c_str()));
	}
	return vec;
}

std::vector<uint64> PropertyTreeSupport::GetAsUInt64List(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	std::vector<uint64> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree.get_child(path))
	{
		vec.push_back(boost::lexical_cast<uint64_t>(v.second.data().c_str()));
	}
	return vec;
}

std::vector<uint64> PropertyTreeSupport::GetAsUInt64List(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	std::vector<uint64> vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(boost::lexical_cast<uint64_t>(v.second.data().c_str()));
	}
	return vec;
}
