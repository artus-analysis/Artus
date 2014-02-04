
#pragma once

#include <string>

class StringHelper {
public:
	static std::string ReadStringFromFile(std::string fileName)
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
	};
};
