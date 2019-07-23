
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
#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Core/interface/MetadataBase.h"

#include "Artus/Core/interface/ConsumerBase.h"

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

template<class TTypes>
class LambdaNtupleConsumer: public ConsumerBase<TTypes> {

public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;
	
	typedef std::function<bool(event_type const&, product_type const&)> bool_extractor_lambda_spec;
	typedef std::function<int(event_type const&, product_type const&)> int_extractor_lambda_spec;
	typedef std::function<uint64_t(event_type const&, product_type const&)> uint64_extractor_lambda_spec;
	typedef std::function<float(event_type const&, product_type const&)> float_extractor_lambda_spec;
	typedef std::function<double(event_type const&, product_type const&)> double_extractor_lambda_spec;
	typedef std::function<ROOT::Math::PtEtaPhiMVector(event_type const&, product_type const&)> ptEtaPhiMVector_extractor_lambda_spec;
	typedef std::function<RMFLV(event_type const&, product_type const&)> rmflv_extractor_lambda_spec;
	typedef std::function<CartesianRMFLV(event_type const&, product_type const&)> cartesianRMFLV_extractor_lambda_spec;
	typedef std::function<RMPoint(event_type const&, product_type const&)> rmpoint_extractor_lambda_spec;
	typedef std::function<std::string(event_type const&, product_type const&)> string_extractor_lambda_spec;
	typedef std::function<std::vector<double>(event_type const&, product_type const&)> vDouble_extractor_lambda_spec;
	typedef std::function<std::vector<float>(event_type const&, product_type const&)> vFloat_extractor_lambda_spec;
	typedef std::function<std::vector<RMFLV>(event_type const&, product_type const&)> vRMFLV_extractor_lambda_spec;
	typedef std::function<std::vector<std::string>(event_type const&, product_type const&)> vString_extractor_lambda_spec;
	typedef std::function<std::vector<int>(event_type const&, product_type const&)> vInt_extractor_lambda_spec;

	static void AddBoolQuantity(metadata_type& metadata, std::string const& name, bool_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonBoolQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> bool
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddIntQuantity(metadata_type& metadata, std::string const& name, int_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonIntQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> int
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddUInt64Quantity(metadata_type& metadata, std::string const& name, uint64_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonUInt64Quantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> uint64_t
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddFloatQuantity(metadata_type& metadata, std::string const& name, float_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonFloatQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> float
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddDoubleQuantity(metadata_type& metadata, std::string const& name, double_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonDoubleQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> double
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddPtEtaPhiMVectorQuantity(metadata_type& metadata, std::string const& name, ptEtaPhiMVector_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonPtEtaPhiMVectorQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> ROOT::Math::PtEtaPhiMVector
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddRMFLVQuantity(metadata_type& metadata, std::string const& name, rmflv_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonRMFLVQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> RMFLV
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddCartesianRMFLVQuantity(metadata_type& metadata, std::string const& name, cartesianRMFLV_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonCartesianRMFLVQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> CartesianRMFLV
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddRMPointQuantity(metadata_type& metadata, std::string const& name, rmpoint_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonRMPointQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> RMPoint
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddStringQuantity(metadata_type& metadata, std::string const& name, string_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonStringQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> std::string
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddVDoubleQuantity(metadata_type& metadata, std::string const& name, vDouble_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonVDoubleQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> std::vector<double>
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddVFloatQuantity(metadata_type& metadata, std::string const& name, vFloat_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonVFloatQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> std::vector<float>
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddVRMFLVQuantity(metadata_type& metadata, std::string const& name, vRMFLV_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonVRMFLVQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> std::vector<RMFLV>
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddVStringQuantity(metadata_type& metadata, std::string const& name, vString_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonVStringQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> std::vector<std::string>
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}
	static void AddVIntQuantity(metadata_type& metadata, std::string const& name, vInt_extractor_lambda_spec valueExtractor)
	{
		metadata.m_commonVIntQuantities[name] = [valueExtractor](EventBase const& event, ProductBase const& product) -> std::vector<int>
		{
			event_type const& specEvent = static_cast<event_type const&>(event);
			product_type const& specProduct = static_cast<product_type const&>(product);
			return valueExtractor(specEvent, specProduct);
		};
	}

	void Init(setting_type const& settings, metadata_type& metadata) override {
		ConsumerBase<TTypes>::Init(settings, metadata);

		// construct value extractors
		m_boolValueExtractors.clear();
		m_floatValueExtractors.clear();
		m_intValueExtractors.clear();
		m_uint64ValueExtractors.clear();
		m_doubleValueExtractors.clear();
		m_ptEtaPhiMVectorValueExtractors.clear();
		m_rmflvValueExtractors.clear();
		m_cartesianRMFLVValueExtractors.clear();
		m_rmpointValueExtractors.clear();
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
		m_rmpointQuantities.clear();
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
			if (metadata.m_commonFloatQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init float quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_floatValueExtractors.push_back(SafeMap::Get(metadata.m_commonFloatQuantities, *quantity));
				m_floatQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonIntQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init int quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_intValueExtractors.push_back(SafeMap::Get(metadata.m_commonIntQuantities, *quantity));
				m_intQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonUInt64Quantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init uint64 quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_uint64ValueExtractors.push_back(SafeMap::Get(metadata.m_commonUInt64Quantities, *quantity));
				m_uint64Quantities.push_back(*quantity);
			}
			else if (metadata.m_commonDoubleQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init double quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_doubleValueExtractors.push_back(SafeMap::Get(metadata.m_commonDoubleQuantities, *quantity));
				m_doubleQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonVDoubleQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vDouble quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vDoubleValueExtractors.push_back(SafeMap::Get(metadata.m_commonVDoubleQuantities, *quantity));
				m_vDoubleQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonVFloatQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vFloat quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vFloatValueExtractors.push_back(SafeMap::Get(metadata.m_commonVFloatQuantities, *quantity));
				m_vFloatQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonBoolQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init bool quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_boolValueExtractors.push_back(SafeMap::Get(metadata.m_commonBoolQuantities, *quantity));
				m_boolQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonVIntQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vBool quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vIntValueExtractors.push_back(SafeMap::Get(metadata.m_commonVIntQuantities, *quantity));
				m_vIntQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonPtEtaPhiMVectorQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init ROOT::Math::PtEtaPhiMVector quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_ptEtaPhiMVectorValueExtractors.push_back(SafeMap::Get(metadata.m_commonPtEtaPhiMVectorQuantities, *quantity));
				m_ptEtaPhiMVectorQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonRMFLVQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init RMFLV quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_rmflvValueExtractors.push_back(SafeMap::Get(metadata.m_commonRMFLVQuantities, *quantity));
				m_rmflvQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonCartesianRMFLVQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init CartesianRMFLV quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_cartesianRMFLVValueExtractors.push_back(SafeMap::Get(metadata.m_commonCartesianRMFLVQuantities, *quantity));
				m_cartesianRMFLVQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonRMPointQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init RMPoint quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_rmpointValueExtractors.push_back(SafeMap::Get(metadata.m_commonRMPointQuantities, *quantity));
				m_rmpointQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonVRMFLVQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vRMFLV quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vRMFLVValueExtractors.push_back(SafeMap::Get(metadata.m_commonVRMFLVQuantities, *quantity));
				m_vRMFLVQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonStringQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init string quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_stringValueExtractors.push_back(SafeMap::Get(metadata.m_commonStringQuantities, *quantity));
				m_stringQuantities.push_back(*quantity);
			}
			else if (metadata.m_commonVStringQuantities.count(*quantity) > 0)
			{
				//LOG(DEBUG) << "Init vString quantity: " <<  << *quantity << " (index " << m_floatValueExtractors.size() << ")");
				m_vStringValueExtractors.push_back(SafeMap::Get(metadata.m_commonVStringQuantities, *quantity));
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
		m_rmpointValues.resize(m_rmpointValueExtractors.size());
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
		size_t rmpointQuantityIndex = 0;
		size_t stringQuantityIndex = 0;
		size_t vDoubleQuantityIndex = 0;
		size_t vFloatQuantityIndex = 0;
		size_t vRMFLVQuantityIndex = 0;
		size_t vStringQuantityIndex = 0;
		size_t vIntQuantityIndex = 0;
		for (std::vector<std::string>::iterator quantity = settings.GetQuantities().begin();
		     quantity != settings.GetQuantities().end(); ++quantity)
		{
			if (metadata.m_commonFloatQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_floatValues[floatQuantityIndex]), (*quantity + "/F").c_str());
				++floatQuantityIndex;
			}
			else if (metadata.m_commonIntQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_intValues[intQuantityIndex]), (*quantity + "/I").c_str());
				++intQuantityIndex;
			}
			else if (metadata.m_commonUInt64Quantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_uint64Values[uint64QuantityIndex]), (*quantity + "/l").c_str());
				++uint64QuantityIndex;
			}
			else if (metadata.m_commonDoubleQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_doubleValues[doubleQuantityIndex]), (*quantity + "/D").c_str());
				++doubleQuantityIndex;
			}
			else if (metadata.m_commonVDoubleQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vDoubleValues[vDoubleQuantityIndex]));
				++vDoubleQuantityIndex;
			}
			else if (metadata.m_commonVFloatQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vFloatValues[vFloatQuantityIndex]));
				++vFloatQuantityIndex;
			}
			else if (metadata.m_commonBoolQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_boolValues[boolQuantityIndex]), (*quantity + "/O").c_str());
				++boolQuantityIndex;
			}
			else if (metadata.m_commonPtEtaPhiMVectorQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), "ROOT::Math::PtEtaPhiMVector", &(m_ptEtaPhiMVectorValues[ptEtaPhiMVectorQuantityIndex]));
				++ptEtaPhiMVectorQuantityIndex;
			}
			else if (metadata.m_commonRMFLVQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_rmflvValues[rmflvQuantityIndex]));
				++rmflvQuantityIndex;
			}
			else if (metadata.m_commonCartesianRMFLVQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_cartesianRMFLVValues[cartesianRMFLVQuantityIndex]));
				++cartesianRMFLVQuantityIndex;
			}
			else if (metadata.m_commonRMPointQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_rmpointValues[rmpointQuantityIndex]));
				++rmpointQuantityIndex;
			}
			else if (metadata.m_commonVRMFLVQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vRMFLVValues[vRMFLVQuantityIndex]));
				++vRMFLVQuantityIndex;
			}
			else if (metadata.m_commonStringQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_stringValues[stringQuantityIndex]));
				++stringQuantityIndex;
			}
			else if (metadata.m_commonVStringQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vStringValues[vStringQuantityIndex]));
				++vStringQuantityIndex;
			}
			else if (metadata.m_commonVIntQuantities.count(*quantity) > 0)
			{
				m_tree->Branch(quantity->c_str(), &(m_vIntValues[vIntQuantityIndex]));
				++vIntQuantityIndex;
			}
		}
	}

	void ProcessFilteredEvent(event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata ) override
	{
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product, settings, metadata);

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

		size_t rmpointValueIndex = 0;
		for(typename std::vector<rmpoint_extractor_lambda_base>::iterator valueExtractor = m_rmpointValueExtractors.begin();
		    valueExtractor != m_rmpointValueExtractors.end(); ++valueExtractor)
		{
			try
			{
				m_rmpointValues[rmpointValueIndex] = (*valueExtractor)(event, product);
			}
			catch (...)
			{
				LOG(FATAL) << "Could not call lambda function for RMPoint quantity \"" << m_rmpointQuantities.at(rmpointValueIndex) << "\" (pipeline \"" << settings.GetName() << "\")!";
			}
			++rmpointValueIndex;
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

	void Finish(setting_type const& settings, metadata_type const& metadata) override
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
	std::vector<rmpoint_extractor_lambda_base> m_rmpointValueExtractors;
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
	std::vector<std::string> m_rmpointQuantities;
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
	std::vector<RMPoint> m_rmpointValues;
	std::vector<std::string> m_stringValues;
	std::vector<std::vector<double> > m_vDoubleValues;
	std::vector<std::vector<float> > m_vFloatValues;
	std::vector<std::vector<RMFLV> > m_vRMFLVValues;
	std::vector<std::vector<RMPoint> > m_vRMPointValues;
	std::vector<std::vector<std::string> > m_vStringValues;
	std::vector<std::vector<int> > m_vIntValues;
};

