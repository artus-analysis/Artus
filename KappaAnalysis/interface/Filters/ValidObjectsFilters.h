
#pragma once

#include "Artus/Filter/interface/CutFilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"



/** Filter checking for the existance of at least one valid electron.
 */
class ValidElectronsFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existance of at least one valid muon.
 */
class ValidMuonsFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existance of at least one valid tau.
 */
class ValidTausFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existance of at least one valid jet.
 */
class ValidJetsFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};


/** Filter checking for the existance of at least one valid b-tagged jet.
 */
class ValidBTaggedJetsFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};

/** Filter checking for the existance of at least one gen-level electron.
 */
class GenElectronsFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};

/** Filter checking for the existance of at least one gen-level muon.
 */
class GenMuonsFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};

/** Filter checking for the existance of at least one gen-level tau.
 */
class GenTausFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};

/** Filter checking for the existance of at least one gen-level tau jet.
 */
class GenTauJetsFilter: public CutRangeFilterBase<KappaTypes> {
public:

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
};
