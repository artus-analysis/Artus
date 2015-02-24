
#include <assert.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Utility/interface/ArtusLogging.h"


stringvector PropertyTreeSupport::GetAsStringList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	stringvector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(v.second.data());
	}
	return vec;
}

doublevector PropertyTreeSupport::GetAsDoubleList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	doublevector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(boost::lexical_cast<double>(v.second.data().c_str()));
	}
	return vec;
}

doublevector PropertyTreeSupport::GetAsDoubleList(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	doublevector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree.get_child(path))
	{
		vec.push_back(boost::lexical_cast<double>(v.second.data().c_str()));
	}
	return vec;
}

floatvector PropertyTreeSupport::GetAsFloatList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	floatvector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(boost::lexical_cast<float>(v.second.data().c_str()));
	}
	return vec;
}

floatvector PropertyTreeSupport::GetAsFloatList(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	floatvector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree.get_child(path))
	{
		vec.push_back(boost::lexical_cast<float>(v.second.data().c_str()));
	}
	return vec;
}

intvector PropertyTreeSupport::GetAsIntList(boost::property_tree::ptree & propTree,
                                            std::string path)
{
	intvector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree.get_child(path))
	{
		vec.push_back(boost::lexical_cast<int>(v.second.data().c_str()));
	}
	return vec;
}

intvector PropertyTreeSupport::GetAsIntList(boost::property_tree::ptree * propTree,
                                            std::string path)
{
	intvector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(boost::lexical_cast<int>(v.second.data().c_str()));
	}
	return vec;
}

uint64vector PropertyTreeSupport::GetAsUInt64List(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	uint64vector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree.get_child(path))
	{
		vec.push_back(boost::lexical_cast<uint64_t>(v.second.data().c_str()));
	}
	return vec;
}

uint64vector PropertyTreeSupport::GetAsUInt64List(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	uint64vector vec;
	BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
			propTree->get_child(path))
	{
		vec.push_back(boost::lexical_cast<uint64_t>(v.second.data().c_str()));
	}
	return vec;
}
