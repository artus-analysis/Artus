/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#include "Artus/KappaTools/interface/IOHelper.h"

void printPTree(const boost::property_tree::ptree &pt, const std::string prefix)
{
	std::cout << prefix << "data = \"" << pt.data() << '"' << std::endl;
	using boost::property_tree::ptree;
	for (ptree::const_iterator it = pt.begin(); it != pt.end(); ++it)
	{
		std::cout << prefix << "child = \""  << it->first << '"' << std::endl;
		printPTree(it->second, prefix + "\t");
	}
}
