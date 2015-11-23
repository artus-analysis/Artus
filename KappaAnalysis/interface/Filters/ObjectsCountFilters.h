
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter checking for the existence of exactly the given number of valid electrons.
 *  Required config tag: NElectrons
 */
class ElectronsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of exactly the given number of valid muons.
 *  Required config tag: NMuons
 */
class MuonsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of exactly the given number of valid taus.
 *  Required config tag: NTaus
 */
class TausCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of exactly the given number of valid jets.
 *  Required config tag: NJets
 */
class JetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;

	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of exactly the given number of valid b-tagged jets.
 *  Required config tag: NBTaggedJets
 */
class BTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;

	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existence of exactly the given number of valid non b-tagged jets.
 *  Required config tag: NNonBTaggedJets
 */
class NonBTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;

	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};
