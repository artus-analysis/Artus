/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

#include "GlobalInclude.h"

/**
   Enum to store whether the input data is MC or Data
*/
enum InputTypeEnum
 {
    McInput, 
    DataInput, 
    GenInput, 
    UnknownInput
 };
