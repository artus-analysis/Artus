
#include <assert.h>

#include <boost/foreach.hpp>

#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Utility/interface/ArtusLogging.h"


stringvector PropertyTreeSupport::GetAsStringList(boost::property_tree::ptree * propTree,
                                                  std::string path, bool failIfNotFound)
{
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
			LOG(FATAL) << "String list on path " << path << " can not be found in configuration file!";
	}
	return fvec;
}

doublevector PropertyTreeSupport::GetAsDoubleList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
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

doublevector PropertyTreeSupport::GetAsDoubleList(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
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

floatvector PropertyTreeSupport::GetAsFloatList(boost::property_tree::ptree * propTree,
                                                  std::string path)
{
	floatvector fvec;
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

floatvector PropertyTreeSupport::GetAsFloatList(boost::property_tree::ptree & propTree,
                                                  std::string path)
{
	floatvector fvec;
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

intvector PropertyTreeSupport::GetAsIntList(boost::property_tree::ptree & propTree,
                                            std::string path)
{
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

intvector PropertyTreeSupport::GetAsIntList(boost::property_tree::ptree * propTree,
                                            std::string path)
{
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
