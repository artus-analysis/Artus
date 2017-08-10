#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > CartesianRMFLV;

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

std::map<std::string, std::function<ROOT::Math::PtEtaPhiMVector(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonPtEtaPhiMVectorQuantities
	= std::map<std::string, std::function<ROOT::Math::PtEtaPhiMVector(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<RMFLV(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonRMFLVQuantities
	= std::map<std::string, std::function<RMFLV(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<CartesianRMFLV(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonCartesianRMFLVQuantities
	= std::map<std::string, std::function<CartesianRMFLV(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<std::string(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonStringQuantities
	= std::map<std::string, std::function<std::string(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<std::vector<double>(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonVDoubleQuantities
	= std::map<std::string, std::function<std::vector<double>(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<std::vector<float>(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonVFloatQuantities
	= std::map<std::string, std::function<std::vector<float>(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<std::vector<RMFLV>(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonVRMFLVQuantities
	= std::map<std::string, std::function<std::vector<RMFLV>(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<std::vector<std::string>(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonVStringQuantities
	= std::map<std::string, std::function<std::vector<std::string>(EventBase const&, ProductBase const& ) >>();

std::map<std::string, std::function<std::vector<int>(EventBase const&, ProductBase const& ) >> LambdaNtupleQuantities::CommonVIntQuantities
	= std::map<std::string, std::function<std::vector<int>(EventBase const&, ProductBase const& ) >>();

