
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter checking for the existence of at least the given number of valid electrons.
 *  Required config tag: MinNElectrons
 */
class MinElectronsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at least the given number of valid muons.
 *  Required config tag: MinNMuons
 */
class MinMuonsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at least the given number of valid taus.
 *  Required config tag: MinNTaus
 */
class MinTausCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at least the given number of valid jets.
 *  Required config tag: MinNJets
 */
class MinJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at least the given number of valid b-tagged jets.
 *  Required config tag: MinNBTaggedJets
 */
class MinBTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existence of at least the given number of valid b-tagged jets.
 *  Required config tag: MinNNonBTaggedJets
 */
class MinNonBTaggedJetsCountFilter: public CutRangeFilterBase<KappaTypes> {
public:
	
	typedef typename std::function<double(event_type const&, product_type const&)> double_extractor_lambda;
	
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};
