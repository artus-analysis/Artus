#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> LCont::CommonQuantities 
	= std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >>(); 


