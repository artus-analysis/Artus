
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"



/** Filter checking for the existance of at least one valid electron.
 */
class ValidElectronsFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existance of at least one valid muon.
 */
class ValidMuonsFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existance of at least one valid tau.
 */
class ValidTausFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existance of at least one valid jet.
 */
class ValidJetsFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};


/** Filter checking for the existance of at least one valid b-tagged jet.
 */
class ValidBTaggedJetsFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(KappaEvent const&, KappaProduct const&)> double_extractor_lambda;
	
	virtual std::string GetFilterId() const override;
	virtual void Init(KappaSettings const& settings) override;
};
