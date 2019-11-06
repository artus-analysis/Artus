#include "Artus/KappaAnalysis/interface/Filters/ObjectsCountFilters.h"

/** Filter checking for the existence of exactly the given number of valid electrons.
 *  Required config tag: NElectrons
 */
	std::string ElectronsCountFilter::GetFilterId() const {
		return "ElectronsCountFilter";
	}

	void ElectronsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
	{
		FilterBase<KappaTypes>::Init(settings, metadata);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
					return product.m_validElectrons.size();
				},
				CutRange::EqualsCut(double(settings.GetNElectrons()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid muons.
 *  Required config tag: NMuons
 */
	std::string MuonsCountFilter::GetFilterId() const
	{
		return "MuonsCountFilter";
	}

	void MuonsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
	{
		FilterBase<KappaTypes>::Init(settings, metadata);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
					return product.m_validMuons.size();
				},
				CutRange::EqualsCut(double(settings.GetNMuons()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid taus.
 *  Required config tag: NTaus
 */
	std::string TausCountFilter::GetFilterId() const
	{
		return "TausCountFilter";
	}

	void TausCountFilter::Init(setting_type const& settings, metadata_type& metadata)
	{
		FilterBase<KappaTypes>::Init(settings, metadata);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
					return product.m_validTaus.size();
				},
				CutRange::EqualsCut(double(settings.GetNTaus()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid jets.
 *  Required config tag: NJets
 */
	std::string JetsCountFilter::GetFilterId() const
	{
		return "JetsCountFilter";
	}

	void JetsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
	{
		FilterBase<KappaTypes>::Init(settings, metadata);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
					return product.m_validJets.size();
				},
				CutRange::EqualsCut(double(settings.GetNJets()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid b-tagged jets.
 *  Required config tag: NBTaggedJets
 */
	std::string BTaggedJetsCountFilter::GetFilterId() const
	{
		return "BTaggedJetsCountFilter";
	}

	void BTaggedJetsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
	{
		FilterBase<KappaTypes>::Init(settings, metadata);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
					return product.m_bTaggedJets.size();
				},
				CutRange::EqualsCut(double(settings.GetNBTaggedJets()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid non b-tagged jets.
 *  Required config tag: NNonBTaggedJets
 */
	std::string NonBTaggedJetsCountFilter::GetFilterId() const
	{
		return "NonBTaggedJetsCountFilter";
	}

	void NonBTaggedJetsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
	{
		FilterBase<KappaTypes>::Init(settings, metadata);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
					return product.m_nonBTaggedJets.size();
				},
				CutRange::EqualsCut(double(settings.GetNNonBTaggedJets()))
		));
	}
