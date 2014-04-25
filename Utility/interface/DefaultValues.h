#pragma once

// a set of default values that can be used in your physics analysis
// be aware. Comparing 2 floating point values like ( a == b ) is 
// never a good idea and the outcome can depend of compile and platform
class DefaultValues {
public:
    constexpr static float  UndefinedFloat = -999.0;
    constexpr static double UndefinedDouble = -999.0;
};
