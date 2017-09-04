
#pragma once

#include <cstdint>
#include <cassert>

#include <boost/algorithm/string/predicate.hpp>

#include <TDirectory.h>
#include <TTree.h>
#include <Math/Vector4D.h>
#include <Math/Vector4Dfwd.h>

#include "Artus/Core/interface/EventBase.h"
#include "Artus/Core/interface/ProductBase.h"
#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/Utility/interface/RootFileHelper.h"


/**
 * Fills TTrees with valueExtractors defined as lambda functions
 * This removes the string operations from its base class
 * This consumer can only be fully initilised in the constructor of an derived class
 * where the map LambdaNtupleConsumer<TTypes>::Quantities is filled with analysis specific code
 */


class LambdaNtupleQuantities {

public:
	typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
	typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > CartesianRMFLV;
	
	static std::map<std::string, std::function<bool(EventBase const&, ProductBase const& ) >> CommonBoolQuantities;
	static std::map<std::string, std::function<int(EventBase const&, ProductBase const& ) >> CommonIntQuantities;
	static std::map<std::string, std::function<uint64_t(EventBase const&, ProductBase const& ) >> CommonUInt64Quantities;
	static std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> CommonFloatQuantities;
	static std::map<std::string, std::function<double(EventBase const&, ProductBase const& ) >> CommonDoubleQuantities;
	static std::map<std::string, std::function<ROOT::Math::PtEtaPhiMVector(EventBase const&, ProductBase const& ) >> CommonPtEtaPhiMVectorQuantities;
	static std::map<std::string, std::function<RMFLV(EventBase const&, ProductBase const& ) >> CommonRMFLVQuantities;
	static std::map<std::string, std::function<CartesianRMFLV(EventBase const&, ProductBase const& ) >> CommonCartesianRMFLVQuantities;
	static std::map<std::string, std::function<std::string(EventBase const&, ProductBase const& ) >> CommonStringQuantities;
	static std::map<std::string, std::function<std::vector<double>(EventBase const&, ProductBase const& ) >> CommonVDoubleQuantities;
	static std::map<std::string, std::function<std::vector<float>(EventBase const&, ProductBase const& ) >> CommonVFloatQuantities;
	static std::map<std::string, std::function<std::vector<RMFLV>(EventBase const&, ProductBase const& ) >> CommonVRMFLVQuantities;
	static std::map<std::string, std::function<std::vector<std::string>(EventBase const&, ProductBase const& ) >> CommonVStringQuantities;
	static std::map<std::string, std::function<std::vector<int>(EventBase const&, ProductBase const& ) >> CommonVIntQuantities;
};

template<class TTypes>
class LambdaNtupleConsumer: public ConsumerBase<TTypes> {

public:
	typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
	typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > CartesianRMFLV;

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef std::function<bool(EventBase const&, ProductBase const&)> bool_extractor_lambda_base;
	typedef std::function<int(EventBase const&, ProductBase const&)> int_extractor_lambda_base;
	typedef std::function<uint64_t(EventBase const&, ProductBase const&)> uint64_extractor_lambda_base;
	typedef std::function<float(EventBase const&, ProductBase const&)> float_extractor_lambda_base;
	typedef std::function<double(EventBase const&, ProductBase const&)> double_extractor_lambda_base;
	typedef std::function<ROOT::Math::PtEtaPhiMVector(EventBase const&, ProductBase const&)> ptEtaPhiMVector_extractor_lambda_base;
	typedef std::function<RMFLV(EventBase const&, ProductBase const&)> rmflv_extractor_lambda_base;
	typedef std::function<CartesianRMFLV(EventBase const&, ProductBase const&)> cartesianRMFLV_extractor_lambda_base;
	typedef std::function<std::string(EventBase const&, ProductBase const&)> string_extractor_lambda_base;
	typedef std::function<std::vector<double>(EventBase const&, ProductBase const&)> vDouble_extractor_lambda_base;
	typedef std::function<std::vector<float>(EventBase const&, ProductBase const&)> vFloat_extractor_lambda_base;
	typedef std::function<std::vector<RMFLV>(EventBase const&, ProductBase const&)> vRMFLV_extractor_lambda_base;
	typedef std::function<std::vector<std::string>(EventBase const&, ProductBase const&)> vString_extractor_lambda_base;
	typedef std::function<std::vector<int>(EventBase const&, ProductBase const&)> vInt_extractor_lambda_base;


	static void AddBoolQuantity(std::string const& name,
	                            std::function<bool(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonBoolQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> bool
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddIntQuantity(std::string const& name,
	                           std::function<int(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonIntQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> int
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddUInt64Quantity(std::string const& name,
	                              std::function<uint64_t(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonUInt64Quantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> uint64_t
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddFloatQuantity(std::string const& name,
	                             std::function<float(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonFloatQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> float
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddDoubleQuantity(std::string const& name,
	                              std::function<double(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonDoubleQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> double
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddPtEtaPhiMVectorQuantity(std::string const& name,
	                              std::function<ROOT::Math::PtEtaPhiMVector(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonPtEtaPhiMVectorQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> ROOT::Math::PtEtaPhiMVector
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddRMFLVQuantity(std::string const& name,
	                              std::function<RMFLV(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonRMFLVQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> RMFLV
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddCartesianRMFLVQuantity(std::string const& name,
	                              std::function<CartesianRMFLV(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonCartesianRMFLVQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> CartesianRMFLV
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddStringQuantity(std::string const& name,
	                              std::function<std::string(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonStringQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> std::string
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddVDoubleQuantity(std::string const& name,
	                              std::function<std::vector<double>(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonVDoubleQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> std::vector<double>
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddVFloatQuantity(std::string const& name,
	                              std::function<std::vector<float>(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonVFloatQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> std::vector<float>
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddVRMFLVQuantity(std::string const& name,
	                              std::function<std::vector<RMFLV>(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonVRMFLVQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> std::vector<RMFLV>
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddVStringQuantity(std::string const& name,
	                              std::function<std::vector<std::string>(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonVStringQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> std::vector<std::string>
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	static void AddVIntQuantity(std::string const& name,
	                              std::function<std::vector<int>(event_type const&, product_type const&)> valueExtractor)
	{
		LambdaNtupleQuantities::CommonVIntQuantities[name] = [valueExtractor](EventBase const& ev, ProductBase const& pd) -> std::vector<int>
		{
			auto const& specEv = static_cast<event_type const&>(ev);
			auto const& specPd = static_cast<product_type const&>(pd);
			return valueExtractor(specEv, specPd);
		};
	}
	

	static std::map<std::string, std::function<bool(EventBase const&, ProductBase const& ) >> & GetBoolQuantities () {
		return LambdaNtupleQuantities::CommonBoolQuantities;
	}
	static std::map<std::string, std::function<int(EventBase const&, ProductBase const& ) >> & GetIntQuantities () {
		return LambdaNtupleQuantities::CommonIntQuantities;
	}
	static std::map<std::string, std::function<uint64_t(EventBase const&, ProductBase const& ) >> & GetUInt64Quantities () {
		return LambdaNtupleQuantities::CommonUInt64Quantities;
	}
	static std::map<std::string, std::function<float(EventBase const&, ProductBase const& ) >> & GetFloatQuantities () {
		return LambdaNtupleQuantities::CommonFloatQuantities;
	}
	static std::map<std::string, std::function<double(EventBase const&, ProductBase const& ) >> & GetDoubleQuantities () {
		return LambdaNtupleQuantities::CommonDoubleQuantities;
	}
	static std::map<std::string, std::function<std::string(EventBase const&, ProductBase const& ) >> & GetStringQuantities () {
		return LambdaNtupleQuantities::CommonStringQuantities;
	}
	static std::map<std::string, std::function<ROOT::Math::PtEtaPhiMVector(EventBase const&, ProductBase const& ) >> & GetPtEtaPhiMVectorQuantities () {
		return LambdaNtupleQuantities::CommonPtEtaPhiMVectorQuantities;
	}
	static std::map<std::string, std::function<RMFLV(EventBase const&, ProductBase const& ) >> & GetRMFLVQuantities () {
		return LambdaNtupleQuantities::CommonRMFLVQuantities;
	}
	static std::map<std::string, std::function<CartesianRMFLV(EventBase const&, ProductBase const& ) >> & GetCartesianRMFLVQuantities () {
		return LambdaNtupleQuantities::CommonCartesianRMFLVQuantities;
	}
	static std::map<std::string, std::function<std::vector<double>(EventBase const&, ProductBase const& ) >> & GetVDoubleQuantities () {
		return LambdaNtupleQuantities::CommonVDoubleQuantities;
	}
	static std::map<std::string, std::function<std::vector<float>(EventBase const&, ProductBase const& ) >> & GetVFloatQuantities () {
		return LambdaNtupleQuantities::CommonVFloatQuantities;
	}
	static std::map<std::string, std::function<std::vector<RMFLV>(EventBase const&, ProductBase const& ) >> & GetVRMFLVQuantities () {
		return LambdaNtupleQuantities::CommonVRMFLVQuantities;
	}
	static std::map<std::string, std::function<std::vector<std::string>(EventBase const&, ProductBase const& ) >> & GetVStringQuantities () {
		return LambdaNtupleQuantities::CommonVStringQuantities;
	}
	static std::map<std::string, std::function<std::vector<int>(EventBase const&, ProductBase const& ) >> & GetVIntQuantities () {
		return LambdaNtupleQuantities::CommonVIntQuantities;
	}

	void Init(setting_type const& settings) override {
		ConsumerBase<TTypes>::Init(settings);

		// construct value extractors
		m_boolValueExtractors.clear();
		m_floatValueExtractors.clear();
		m_intValueExtractors.clear();
		m_uint64ValueExtractors.clear();
		m_doubleValueExtractors.clear();
		m_ptEtaPhiMVectorValueExtractors.clear();
		m_rmflvValueExtractors.clear();
		m_cartesianRMFLVValueExtractors.clear();
		m_stringValueExtractors.clear();
		m_vDoubleValueExtractors.clear();
		m_vFloatValueExtractors.clear();
		m_vRMFLVValueExtractors.clear();
		m_vStringValueExtractors.clear();
		m_vIntValueExtractors.clear();
		
		m_boolQuantities.clear();
		m_intQuantities.clear();
		m_uint64Quantities.clear();
		m_floatQuantities.clear();
		m_doubleQuantities.clear();
		m_ptEtaPhiMVectorQuantities.clear();
		m_rmflvQuantities.clear();
		m_cartesianRMFLVQuantities.clear();
		m_stringQuantities.clear();
		m_vDoubleQuantities.clear();
		m_vFloatQuantities.clear();
		m_vRMFLVQuantities.clear();
		m_vStringQuantities.clear();
		m_vIntQuantities.clear();
		
		size_t quantityIndex = 0;
		for (std::vector<std::string>::iterator quantity = settings.GetQuantities().begin();
		     quantity != settings.GetQuantities().end(); ++quantity)
		{
			if (LambdaNtupleConsumer<TTypes>::GetFloatQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init float quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_floatValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetFloatQuantities(), *quantity));
				m_floatQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetIntQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init int quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_intValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetIntQuantities(), *quantity));
				m_intQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetUInt64Quantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init uint64 quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_uint64ValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetUInt64Quantities(), *quantity));
				m_uint64Quantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetDoubleQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init double quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_doubleValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetDoubleQuantities(), *quantity));
				m_doubleQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetVDoubleQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vDouble quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vDoubleValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetVDoubleQuantities(), *quantity));
				m_vDoubleQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetVFloatQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vFloat quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vFloatValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetVFloatQuantities(), *quantity));
				m_vFloatQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetBoolQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init bool quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_boolValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetBoolQuantities(), *quantity));
				m_boolQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetVIntQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vBool quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vIntValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetVIntQuantities(), *quantity));
				m_vIntQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetPtEtaPhiMVectorQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init ROOT::Math::PtEtaPhiMVector quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_ptEtaPhiMVectorValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetPtEtaPhiMVectorQuantities(), *quantity));
				m_ptEtaPhiMVectorQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetRMFLVQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init RMFLV quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_rmflvValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetRMFLVQuantities(), *quantity));
				m_rmflvQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetCartesianRMFLVQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init CartesianRMFLV quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_cartesianRMFLVValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetCartesianRMFLVQuantities(), *quantity));
				m_cartesianRMFLVQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetVRMFLVQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vRMFLV quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vRMFLVValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetVRMFLVQuantities(), *quantity));
				m_vRMFLVQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetStringQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init string quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_stringValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetStringQuantities(), *quantity));
				m_stringQuantities.push_back(*quantity);
			}
			else if (LambdaNtupleConsumer<TTypes>::GetVStringQuantities().count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vString quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vStringValueExtractors.push_back(SafeMap::Get(LambdaNtupleConsumer<TTypes>::GetVStringQuantities(), *quantity));
				m_vStringQuantities.push_back(*quantity);
			}
			else
			{
				LOG(FATAL) << "No lambda expression available for quantity \"" << *quantity << "\" (pipeline \"" << settings.GetName() << "\")!";
			}

			++quantityIndex;
		}

		// create tree
		TDirectory* tmpDirectory = gDirectory;
		RootFileHelper::SafeCd(settings.GetRootOutFile(), settings.GetRootFileFolder());
		m_tree = new TTree("ntuple", ("Tree for Pipeline \"" + settings.GetName() + "\"").c_str());
		gDirectory = tmpDirectory;

		// create branches
		m_boolValues.resize(m_boolValueExtractors.size());
		m_intValues.resize(m_intValueExtractors.size());
		m_uint64Values.resize(m_uint64ValueExtractors.size());
		m_floatValues.resize(m_floatValueExtractors.size());
		m_doubleValues.resize(m_doubleValueExtractors.size());
		m_ptEtaPhiMVectorValues.resize(m_ptEtaPhiMVectorValueExtractors.size());
		m_rmflvValues.resize(m_rmflvValueExtractors.size());
		m_cartesianRMFLVValues.resize(m_cartesianRMFLVValueExtractors.size());
		m_stringValues.resize(m_stringValueExtractors.size());
		m_vDoubleValues.resize(m_vDoubleValueExtractors.size());
		m_vFloatValues.resize(m_vFloatValueExtractors.size());
		m_vRMFLVValues.resize(m_vRMFLVValueExtractors.size());
		m_vStringValues.resize(m_vStringValueExtractors.size());
		m_vIntValues.resize(m_vIntValueExtractors.size());

		size_t boolQuantityIndex = 0;
		size_t intQuantityIndex = 0;
		size_t uint64QuantityIndex = 0;
		size_t floatQuantityIndex = 0;
		size_t doubleQuantityIndex = 0;
		size_t ptEtaPhiMVectorQuantityIndex = 0;
		size_t rmflvQuantityIndex = 0;
		size_t cartesianRMFLVQuantityIndex = 0;
		size_t stringQuantityIndex = 0;
		size_t vDoubleQuantityIndex = 0;
		size_t vFloatQuantityIndex = 0;
		size_t vRMFLVQuantityIndex = 0;
		size_t vStringQuantityIndex = 0;
		size_t vIntQuantityIndex = 0;
		for (std::vector<std::string>::iterator quantity = settings.GetQuantities().begin();
		     quantity != settings.GetQuantities().end(); ++quantity)
		{
			if (LambdaNtupleQuantities::CommonFloatQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_floatValues[floatQuantityIndex]), (*quantity + "/F").c_str());
				++floatQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonIntQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_intValues[intQuantityIndex]), (*quantity + "/I").c_str());
				++intQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonUInt64Quantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_uint64Values[uint64QuantityIndex]), (*quantity + "/l").c_str());
				++uint64QuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonDoubleQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_doubleValues[doubleQuantityIndex]), (*quantity + "/D").c_str());
				++doubleQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonVDoubleQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vDoubleValues[vDoubleQuantityIndex]));
				++vDoubleQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonVFloatQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vFloatValues[vFloatQuantityIndex]));
				++vFloatQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonBoolQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_boolValues[boolQuantityIndex]), (*quantity + "/O").c_str());
				++boolQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonPtEtaPhiMVectorQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), "ROOT::Math::PtEtaPhiMVector", &(m_ptEtaPhiMVectorValues[ptEtaPhiMVectorQuantityIndex]));
				++ptEtaPhiMVectorQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonRMFLVQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_rmflvValues[rmflvQuantityIndex]));
				++rmflvQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonCartesianRMFLVQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_cartesianRMFLVValues[cartesianRMFLVQuantityIndex]));
				++cartesianRMFLVQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonVRMFLVQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vRMFLVValues[vRMFLVQuantityIndex]));
				++vRMFLVQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonStringQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_stringValues[stringQuantityIndex]));
				++stringQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonVStringQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vStringValues[vStringQuantityIndex]));
				++vStringQuantityIndex;
			}
			else if (LambdaNtupleQuantities::CommonVIntQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vIntValues[vIntQuantityIndex]));
				++vIntQuantityIndex;
			}
		}
	}

	void ProcessFilteredEvent(event_type const& event, product_type const& product, setting_type const& settings ) override
	{
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product, settings);

		// calculate values
		size_t boolValueIndex = 0;
		for(typename std::vector<bool_extractor_lambda_base>::iterator valueExtractor = m_boolValueExtractors.begin();
		    valueExtractor != m_boolValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_boolValues[boolValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for bool quantity \"" << m_boolQuantities.at(boolValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++boolValueIndex;
		}

		size_t intValueIndex = 0;
		for(typename std::vector<int_extractor_lambda_base>::iterator valueExtractor = m_intValueExtractors.begin();
		    valueExtractor != m_intValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_intValues[intValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for int quantity \"" << m_intQuantities.at(intValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++intValueIndex;
		}

		size_t uint64ValueIndex = 0;
		for(typename std::vector<uint64_extractor_lambda_base>::iterator valueExtractor = m_uint64ValueExtractors.begin();
		    valueExtractor != m_uint64ValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_uint64Values[uint64ValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for uint64 quantity \"" << m_uint64Quantities.at(uint64ValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++uint64ValueIndex;
		}

		size_t floatValueIndex = 0;
		for(typename std::vector<float_extractor_lambda_base>::iterator valueExtractor = m_floatValueExtractors.begin();
		    valueExtractor != m_floatValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_floatValues[floatValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for float quantity \"" << m_floatQuantities.at(floatValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++floatValueIndex;
		}

		size_t doubleValueIndex = 0;
		for(typename std::vector<double_extractor_lambda_base>::iterator valueExtractor = m_doubleValueExtractors.begin();
		    valueExtractor != m_doubleValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_doubleValues[doubleValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for double quantity \"" << m_doubleQuantities.at(doubleValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++doubleValueIndex;
		}
		
		size_t ptEtaPhiMVectorValueIndex = 0;
		for(typename std::vector<ptEtaPhiMVector_extractor_lambda_base>::iterator valueExtractor = m_ptEtaPhiMVectorValueExtractors.begin();
		    valueExtractor != m_ptEtaPhiMVectorValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_ptEtaPhiMVectorValues[ptEtaPhiMVectorValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for ROOT::Math::PtEtaPhiMVector quantity \"" << m_ptEtaPhiMVectorQuantities.at(ptEtaPhiMVectorValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++ptEtaPhiMVectorValueIndex;
		}
		
		size_t rmflvValueIndex = 0;
		for(typename std::vector<rmflv_extractor_lambda_base>::iterator valueExtractor = m_rmflvValueExtractors.begin();
		    valueExtractor != m_rmflvValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_rmflvValues[rmflvValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for RMFLV quantity \"" << m_rmflvQuantities.at(rmflvValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++rmflvValueIndex;
		}
		
		size_t cartesianRMFLVValueIndex = 0;
		for(typename std::vector<cartesianRMFLV_extractor_lambda_base>::iterator valueExtractor = m_cartesianRMFLVValueExtractors.begin();
		    valueExtractor != m_cartesianRMFLVValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_cartesianRMFLVValues[cartesianRMFLVValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for CartesianRMFLV quantity \"" << m_cartesianRMFLVQuantities.at(cartesianRMFLVValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++cartesianRMFLVValueIndex;
		}
		
		size_t stringValueIndex = 0;
		for(typename std::vector<string_extractor_lambda_base>::iterator valueExtractor = m_stringValueExtractors.begin();
		    valueExtractor != m_stringValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_stringValues[stringValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for string quantity \"" << m_stringQuantities.at(stringValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++stringValueIndex;
		}
		
		size_t vDoubleValueIndex = 0;
		for(typename std::vector<vDouble_extractor_lambda_base>::iterator valueExtractor = m_vDoubleValueExtractors.begin();
		    valueExtractor != m_vDoubleValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_vDoubleValues[vDoubleValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for vDouble quantity \"" << m_vDoubleQuantities.at(vDoubleValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++vDoubleValueIndex;
		}
		
		size_t vFloatValueIndex = 0;
		for(typename std::vector<vFloat_extractor_lambda_base>::iterator valueExtractor = m_vFloatValueExtractors.begin();
		    valueExtractor != m_vFloatValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_vFloatValues[vFloatValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for vFloat quantity \"" << m_vFloatQuantities.at(vFloatValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++vFloatValueIndex;
		}
		
		size_t vRMFLVValueIndex = 0;
		for(typename std::vector<vRMFLV_extractor_lambda_base>::iterator valueExtractor = m_vRMFLVValueExtractors.begin();
		    valueExtractor != m_vRMFLVValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_vRMFLVValues[vRMFLVValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for vRMFLV quantity \"" << m_vRMFLVQuantities.at(vRMFLVValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++vRMFLVValueIndex;
		}
		
		size_t vStringValueIndex = 0;
		for(typename std::vector<vString_extractor_lambda_base>::iterator valueExtractor = m_vStringValueExtractors.begin();
		    valueExtractor != m_vStringValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_vStringValues[vStringValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for vString quantity \"" << m_vStringQuantities.at(vStringValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++vStringValueIndex;
		}

		size_t vIntValueIndex = 0;
		for(typename std::vector<vInt_extractor_lambda_base>::iterator valueExtractor = m_vIntValueExtractors.begin();
		    valueExtractor != m_vIntValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_vIntValues[vIntValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for vInt quantity \"" << m_vIntQuantities.at(vIntValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++vIntValueIndex;
		}

		// fill tree
		this->m_tree->Fill();
	}

	void Finish(setting_type const& settings) override
	{
		RootFileHelper::SafeCd(settings.GetRootOutFile(), settings.GetRootFileFolder());
		m_tree->Write(m_tree->GetName());
	}


private:
	TTree* m_tree = nullptr;

	std::vector<bool_extractor_lambda_base> m_boolValueExtractors;
	std::vector<int_extractor_lambda_base> m_intValueExtractors;
	std::vector<uint64_extractor_lambda_base> m_uint64ValueExtractors;
	std::vector<float_extractor_lambda_base> m_floatValueExtractors;
	std::vector<double_extractor_lambda_base> m_doubleValueExtractors;
	std::vector<ptEtaPhiMVector_extractor_lambda_base> m_ptEtaPhiMVectorValueExtractors;
	std::vector<rmflv_extractor_lambda_base> m_rmflvValueExtractors;
	std::vector<cartesianRMFLV_extractor_lambda_base> m_cartesianRMFLVValueExtractors;
	std::vector<string_extractor_lambda_base> m_stringValueExtractors;
	std::vector<vDouble_extractor_lambda_base> m_vDoubleValueExtractors;
	std::vector<vFloat_extractor_lambda_base> m_vFloatValueExtractors;
	std::vector<vRMFLV_extractor_lambda_base> m_vRMFLVValueExtractors;
	std::vector<vString_extractor_lambda_base> m_vStringValueExtractors;
	std::vector<vInt_extractor_lambda_base> m_vIntValueExtractors;

	std::vector<std::string> m_boolQuantities;
	std::vector<std::string> m_intQuantities;
	std::vector<std::string> m_uint64Quantities;
	std::vector<std::string> m_floatQuantities;
	std::vector<std::string> m_doubleQuantities;
	std::vector<std::string> m_ptEtaPhiMVectorQuantities;
	std::vector<std::string> m_rmflvQuantities;
	std::vector<std::string> m_cartesianRMFLVQuantities;
	std::vector<std::string> m_stringQuantities;
	std::vector<std::string> m_vDoubleQuantities;
	std::vector<std::string> m_vFloatQuantities;
	std::vector<std::string> m_vRMFLVQuantities;
	std::vector<std::string> m_vStringQuantities;
	std::vector<std::string> m_vIntQuantities;

	std::vector<char> m_boolValues; // needs to be char vector because of bitset treatment of bool vector
	std::vector<int> m_intValues;
	std::vector<uint64_t> m_uint64Values;
	std::vector<float> m_floatValues;
	std::vector<double> m_doubleValues;
	std::vector<ROOT::Math::PtEtaPhiMVector> m_ptEtaPhiMVectorValues;
	std::vector<RMFLV> m_rmflvValues;
	std::vector<CartesianRMFLV> m_cartesianRMFLVValues;
	std::vector<std::string> m_stringValues;
	std::vector<std::vector<double> > m_vDoubleValues;
	std::vector<std::vector<float> > m_vFloatValues;
	std::vector<std::vector<RMFLV> > m_vRMFLVValues;
	std::vector<std::vector<std::string> > m_vStringValues;
	std::vector<std::vector<int> > m_vIntValues;
};

