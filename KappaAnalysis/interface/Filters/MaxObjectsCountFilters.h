
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"


/** Filter checking for the existance of at most the given number of valid electrons.
 *  Required config tag: MaxNElectrons
 */
class MaxElectronsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "MaxElectronsCountFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNElectrons()))
		));
	}
};


/** Filter checking for the existance of at most the given number of valid muons.
 *  Required config tag: MaxNMuons
 */
class MaxMuonsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "MaxMuonsCountFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validMuons.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNMuons()))
		));
	}
};


/** Filter checking for the existance of at most the given number of valid taus.
 *  Required config tag: MaxNTaus
 */
class MaxTausCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "MaxTausCountFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validTaus.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNTaus()))
		));
	}
};


/** Filter checking for the existance of at most the given number of valid jets.
 *  Required config tag: MaxNJets
 */
class MaxJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "MaxJetsCountFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validJets.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNJets()))
		));
	}
};

