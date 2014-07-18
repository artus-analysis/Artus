
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"


/** Filter checking for the existance of exactly the given number of valid electrons.
 *  Required config tag: MaxNElectrons
 */
template<class TTypes>
class MaxElectronsCountFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "max_n_electrons";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNElectrons()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid muons.
 *  Required config tag: MaxNMuons
 */
template<class TTypes>
class MaxMuonsCountFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "max_n_muons";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validMuons.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNMuons()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid taus.
 *  Required config tag: MaxNTaus
 */
template<class TTypes>
class MaxTausCountFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "max_n_taus";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validTaus.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNTaus()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid jets.
 *  Required config tag: MaxNJets
 */
template<class TTypes>
class MaxJetsCountFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "max_n_jets";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validJets.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNJets()))
		));
	}
};

