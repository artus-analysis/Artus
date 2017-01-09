
#pragma once
#include <boost/noncopyable.hpp>

enum class ProcessNodeType {
	Filter,
	Producer,
	Consumer
};

class ProcessNodeBase: public boost::noncopyable {
public:

	virtual ~ProcessNodeBase();

	virtual  ProcessNodeType GetProcessNodeType () const = 0;
};
