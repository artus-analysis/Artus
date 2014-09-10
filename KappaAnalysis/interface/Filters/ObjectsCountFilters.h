
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"


/** Filter checking for the existance of exactly the given number of valid electrons.
 *  Required config tag: NElectrons
 */
class ElectronsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "ElectronsCountFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::EqualsCut(double(settings.GetNElectrons()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid muons.
 *  Required config tag: NMuons
 */
class MuonsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "MuonsCountFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validMuons.size();
				},
				CutRange::EqualsCut(double(settings.GetNMuons()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid taus.
 *  Required config tag: NTaus
 */
class TausCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "TausCountFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validTaus.size();
				},
				CutRange::EqualsCut(double(settings.GetNTaus()))
		));
	}
};


/** Filter checking for the existance of exactly the given number of valid jets.
 *  Required config tag: NJets
 */
class JetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "JetsCountFilter";
	}
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validJets.size();
				},
				CutRange::EqualsCut(double(settings.GetNJets()))
		));
	}
};

