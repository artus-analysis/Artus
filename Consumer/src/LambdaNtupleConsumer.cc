#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


std::map<std::string, std::function<bool(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonBoolQuantities
	= std::map<std::string, std::function<bool(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<int(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonIntQuantities
	= std::map<std::string, std::function<int(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<uint64_t(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonUInt64Quantities
	= std::map<std::string, std::function<uint64_t(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonFloatQuantities
	= std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<double(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonDoubleQuantities
	= std::map<std::string, std::function<double(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<std::vector<double>(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonVDoubleQuantities
	= std::map<std::string, std::function<std::vector<double>(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<std::string(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonStringQuantities
	= std::map<std::string, std::function<std::string(EventBase const&, ProductBase const& ) >>();

