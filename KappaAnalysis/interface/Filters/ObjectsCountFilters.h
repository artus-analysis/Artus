
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"


/** Filter checking for the existance of exactly the given number of valid electrons.
 *  Required config tag: NElectrons
 */
template<class TTypes>
class ElectronsCountFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "n_electrons";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::EqualsCut(double(settings.GetNElectrons()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid muons.
 *  Required config tag: NMuons
 */
template<class TTypes>
class MuonsCountFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "n_muons";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validMuons.size();
				},
				CutRange::EqualsCut(double(settings.GetNMuons()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid taus.
 *  Required config tag: NTaus
 */
template<class TTypes>
class TausCountFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "n_taus";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validTaus.size();
				},
				CutRange::EqualsCut(double(settings.GetNTaus()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid jets.
 *  Required config tag: NJets
 */
template<class TTypes>
class JetsCountFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "n_jets";
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validJets.size();
				},
				CutRange::EqualsCut(double(settings.GetNJets()))
		));
	}
};

