
#pragma once

#include <string>

#include <boost/noncopyable.hpp>


enum class ProcessNodeType {
	Filter,
	Producer,
	Consumer
};

class ProcessNodeBase: public boost::noncopyable {
public:

	virtual ~ProcessNodeBase();
	
	virtual std::string GetId() const;

	virtual  ProcessNodeType GetProcessNodeType () const = 0;
};
