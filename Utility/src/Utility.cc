
#include <fstream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/Utility/interface/Utility.h"


std::string Utility::ReadStringFromFile(std::string fileName)
{
	std::ifstream inStream(fileName, std::ios::in | std::ios::binary);
	if(inStream)
	{
		std::string content;
		inStream.seekg(0, std::ios::end);
		content.resize(inStream.tellg());
		inStream.seekg(0, std::ios::beg);
		inStream.read(&content[0], content.size());
		inStream.close();
		return(content);
	}
	throw(errno);
}

std::map<std::string, std::vector<std::string> > Utility::ParseVectorToMap(std::vector<std::string> const& vectorToParse,
                                                                           std::string const& delimiters)
{
	std::vector<std::string> defaultVector;
	std::map<std::string, std::vector<std::string> > parsedVectors;

	for (std::vector<std::string>::const_iterator item = vectorToParse.begin();
		 item != vectorToParse.end(); ++item)
	{
		std::vector<std::string> splitted;
		boost::algorithm::split(splitted, *item, boost::algorithm::is_any_of(delimiters));
		transform(splitted.begin(), splitted.end(), splitted.begin(),
				  [](std::string s) { return boost::algorithm::trim_copy(s); });
		
		std::string key((splitted.size() > 1) ? splitted[0] : "default");
		std::string value((splitted.size() > 1) ? splitted[1] : splitted[0]);
	
		if (parsedVectors.count(key) == 0) {
			parsedVectors[key] = std::vector<std::string>();
		}
		parsedVectors[key].push_back(value);
	}

	return parsedVectors;
}
