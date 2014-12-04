
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter checking for the existance of at most the given number of valid electrons.
 *  Required config tag: MinNElectrons
 */
class MinElectronsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;
};


/** Filter checking for the existance of at most the given number of valid muons.
 *  Required config tag: MinNMuons
 */
class MinMuonsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;
};


/** Filter checking for the existance of at most the given number of valid taus.
 *  Required config tag: MinNTaus
 */
class MinTausCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;
};


/** Filter checking for the existance of at most the given number of valid jets.
 *  Required config tag: MinNJets
 */
class MinJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;
};

