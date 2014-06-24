#pragma once

// a set of default values that can be used in your physics analysis
// be aware. Comparing 2 floating point values like ( a == b ) is 
// never a good idea and the outcome can depend of compile and platform
class DefaultValues {
public:
	// values set in .cc file
    static const int UndefinedInt;
    static const float UndefinedFloat;
    static const double UndefinedDouble;
    
    static const float EtaBorderEB;

    static const int pdgIdPiZero;
    static const int pdgIdW;
    static const int pdgIdH;
    static const int pdgIdTau;
};
