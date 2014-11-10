#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


std::map<std::string, std::function<bool(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonBoolQuantities 
	= std::map<std::string, std::function<bool(EventBase const&, ProductBase const& ) >>();
	
std::map<std::string, std::function<int(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonIntQuantities 
	= std::map<std::string, std::function<int(EventBase const&, ProductBase const& ) >>();
	
std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonFloatQuantities 
	= std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >>();
	
std::map<std::string, std::function<double(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonDoubleQuantities 
	= std::map<std::string, std::function<double(EventBase const&, ProductBase const& ) >>();


