#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonQuantities 
	= std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >>(); 


