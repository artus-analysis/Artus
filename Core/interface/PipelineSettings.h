
#pragma once

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

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
