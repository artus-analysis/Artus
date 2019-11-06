#pragma once

#include <functional>

#include <Math/Vector4D.h>
#include <Math/Vector4Dfwd.h>
#include <Math/Point3D.h>

#include "Artus/Core/interface/EventBase.h"
#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Configuration/interface/SettingsBase.h"


typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > CartesianRMFLV;
typedef ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<float> > RMPoint;

// Forward declaration
class MetadataBase;

typedef std::function<bool(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> bool_extractor_lambda_base;
typedef std::function<int(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> int_extractor_lambda_base;
typedef std::function<uint64_t(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> uint64_extractor_lambda_base;
typedef std::function<float(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> float_extractor_lambda_base;
typedef std::function<double(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> double_extractor_lambda_base;
typedef std::function<ROOT::Math::PtEtaPhiMVector(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> ptEtaPhiMVector_extractor_lambda_base;
typedef std::function<RMFLV(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> rmflv_extractor_lambda_base;
typedef std::function<CartesianRMFLV(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> cartesianRMFLV_extractor_lambda_base;
typedef std::function<RMPoint(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> rmpoint_extractor_lambda_base;
typedef std::function<std::string(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> string_extractor_lambda_base;
typedef std::function<std::vector<double>(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> vDouble_extractor_lambda_base;
typedef std::function<std::vector<float>(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> vFloat_extractor_lambda_base;
typedef std::function<std::vector<RMFLV>(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> vRMFLV_extractor_lambda_base;
typedef std::function<std::vector<std::string>(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> vString_extractor_lambda_base;
typedef std::function<std::vector<int>(EventBase const&, ProductBase const&, SettingsBase const&, MetadataBase const&)> vInt_extractor_lambda_base;


class MetadataBase
{
public:
	MetadataBase();
	virtual ~MetadataBase();

	std::map<std::string, bool_extractor_lambda_base> m_commonBoolQuantities;
	std::map<std::string, int_extractor_lambda_base> m_commonIntQuantities;
	std::map<std::string, uint64_extractor_lambda_base> m_commonUInt64Quantities;
	std::map<std::string, float_extractor_lambda_base> m_commonFloatQuantities;
	std::map<std::string, double_extractor_lambda_base> m_commonDoubleQuantities;
	std::map<std::string, ptEtaPhiMVector_extractor_lambda_base> m_commonPtEtaPhiMVectorQuantities;
	std::map<std::string, rmflv_extractor_lambda_base> m_commonRMFLVQuantities;
	std::map<std::string, cartesianRMFLV_extractor_lambda_base> m_commonCartesianRMFLVQuantities;
	std::map<std::string, rmpoint_extractor_lambda_base> m_commonRMPointQuantities;
	std::map<std::string, string_extractor_lambda_base> m_commonStringQuantities;
	std::map<std::string, vDouble_extractor_lambda_base> m_commonVDoubleQuantities;
	std::map<std::string, vFloat_extractor_lambda_base> m_commonVFloatQuantities;
	std::map<std::string, vRMFLV_extractor_lambda_base> m_commonVRMFLVQuantities;
	std::map<std::string, vString_extractor_lambda_base> m_commonVStringQuantities;
	std::map<std::string, vInt_extractor_lambda_base> m_commonVIntQuantities;
};
