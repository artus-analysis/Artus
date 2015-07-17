
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter checking for the existence of at most the given number of valid electrons.
 *  Required config tag: MaxNElectrons
 */
class MaxElectronsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of at most the given number of valid muons.
 *  Required config tag: MaxNMuons
 */
class MaxMuonsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of at most the given number of valid taus.
 *  Required config tag: MaxNTaus
 */
class MaxTausCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of at most the given number of valid jets.
 *  Required config tag: MaxNJets
 */
class MaxJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of at most the given number of valid b-tagged jets.
 *  Required config tag: MaxNBTaggedJets
 */
class MaxBTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};

