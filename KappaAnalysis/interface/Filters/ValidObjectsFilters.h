
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"


/** Filter checking for the existance of at least one valid electron.
 */
class ValidElectronsFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "ValidElectronsFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::LowerThresholdCut(1.0)
		));
	}
};


/** Filter checking for the existance of at least one valid muon.
 */
class ValidMuonsFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "ValidMuonsFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validMuons.size();
				},
				CutRange::LowerThresholdCut(1.0)
		));
	}
};


/** Filter checking for the existance of at least one valid tau.
 */
class ValidTausFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "ValidTausFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validTaus.size();
				},
				CutRange::LowerThresholdCut(1.0)
		));
	}
};


/** Filter checking for the existance of at least one valid jet.
 */
class ValidJetsFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "ValidJetsFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validJets.size();
				},
				CutRange::LowerThresholdCut(1.0)
		));
	}
};

