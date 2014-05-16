
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
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "n_electrons";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(globalSettings.GetNElectrons());
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(settings.GetNElectrons());
	}

private:
	void Init(int nElectrons) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::EqualsCut(double(nElectrons))
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
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "n_muons";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(globalSettings.GetNMuons());
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(settings.GetNMuons());
	}

private:
	void Init(int nMuons) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validMuons.size();
				},
				CutRange::EqualsCut(double(nMuons))
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
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "n_taus";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(globalSettings.GetNTaus());
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(settings.GetNTaus());
	}

private:
	void Init(int nTaus) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validTaus.size();
				},
				CutRange::EqualsCut(double(nTaus))
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
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "n_jets";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init(globalSettings.GetNJets());
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init(settings.GetNJets());
	}

private:
	void Init(int nJets) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validJets.size();
				},
				CutRange::EqualsCut(double(nJets))
		));
	}
};

