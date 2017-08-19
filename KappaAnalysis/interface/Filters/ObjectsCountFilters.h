
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter checking for the existence of exactly the given number of valid electrons.
 *  Required config tag: NElectrons
 */
class ElectronsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of exactly the given number of valid muons.
 *  Required config tag: NMuons
 */
class MuonsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of exactly the given number of valid taus.
 *  Required config tag: NTaus
 */
class TausCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of exactly the given number of valid jets.
 *  Required config tag: NJets
 */
class JetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of exactly the given number of valid b-tagged jets.
 *  Required config tag: NBTaggedJets
 */
class BTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of exactly the given number of valid non b-tagged jets.
 *  Required config tag: NNonBTaggedJets
 */
class NonBTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};
