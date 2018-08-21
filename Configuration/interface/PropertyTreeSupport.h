
#pragma once

#include <cstdint>

#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <map>

/*
 * This class provides various convenience functions when working with Boost PropertyTrees.
 */
class PropertyTreeSupport {
public:
	static std::vector<std::string> GetAsStringList(boost::property_tree::ptree * propTree, std::string path);

	static std::vector<double> GetAsDoubleList(boost::property_tree::ptree * propTree, std::string path);

	static std::vector<double> GetAsDoubleList(boost::property_tree::ptree & propTree, std::string path);

	static std::vector<float> GetAsFloatList(boost::property_tree::ptree * propTree, std::string path);

	static std::vector<float> GetAsFloatList(boost::property_tree::ptree & propTree, std::string path);

	static std::vector<int> GetAsIntList(boost::property_tree::ptree & propTree, std::string path);

	static std::vector<int> GetAsIntList(boost::property_tree::ptree * propTree, std::string path);

	static std::vector<uint64_t> GetAsUInt64List(boost::property_tree::ptree & propTree, std::string path);

	static std::vector<uint64_t> GetAsUInt64List(boost::property_tree::ptree * propTree, std::string path);
	
	static std::map<std::string, std::vector<std::string>> GetAsMapStringToListOfStrings(boost::property_tree::ptree * propTree, std::string path);
};
