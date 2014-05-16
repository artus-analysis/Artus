
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"


/** Filter checking for the existance of at least one valid electron.
 */
template<class TTypes>
class ValidElectronsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "valid_electrons";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init();
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init();
	}

private:
	void Init() {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::LowerThresholdCut(1.0)
		));
	}
};


/** Filter checking for the existance of at least one valid muon.
 */
template<class TTypes>
class ValidMuonsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "valid_muons";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init();
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init();
	}

private:
	void Init() {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validMuons.size();
				},
				CutRange::LowerThresholdCut(1.0)
		));
	}
};


/** Filter checking for the existance of at least one valid tau.
 */
template<class TTypes>
class ValidTausFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "valid_taus";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init();
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init();
	}

private:
	void Init() {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validTaus.size();
				},
				CutRange::LowerThresholdCut(1.0)
		));
	}
};


/** Filter checking for the existance of at least one valid jet.
 */
template<class TTypes>
class ValidJetsFilter: public CutRangeFilterBase<TTypes> {
public:
	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "valid_jets";
	}
	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		Init();
	}
	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		Init();
	}

private:
	void Init() {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product) {
					return product.m_validJets.size();
				},
				CutRange::LowerThresholdCut(1.0)
		));
	}
};

