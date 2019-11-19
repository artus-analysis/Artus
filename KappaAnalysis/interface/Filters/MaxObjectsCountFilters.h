
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter checking for the existence of at most the given number of valid electrons.
 *  Required config tag: MaxNElectrons
 */
class MaxElectronsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at most the given number of valid muons.
 *  Required config tag: MaxNMuons
 */
class MaxMuonsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at most the given number of valid taus.
 *  Required config tag: MaxNTaus
 */
class MaxTausCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at most the given number of valid jets.
 *  Required config tag: MaxNJets
 */
class MaxJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at most the given number of valid b-tagged jets.
 *  Required config tag: MaxNBTaggedJets
 */
class MaxBTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at most the given number of valid b-tagged jets.
 *  Required config tag: MaxNNonBTaggedJets
 */
class MaxNonBTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};
