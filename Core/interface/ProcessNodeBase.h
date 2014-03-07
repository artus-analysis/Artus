/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once
#include <boost/noncopyable.hpp>

enum class ProcessNodeType {
	Filter,
	Producer,
	Consumer
};

class ProcessNodeBase: public boost::noncopyable {
public:

	virtual ~ProcessNodeBase() {};

	virtual  ProcessNodeType GetProcessNodeType () const = 0;
};
