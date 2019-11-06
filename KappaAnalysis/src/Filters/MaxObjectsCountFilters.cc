#include "Artus/KappaAnalysis/interface/Filters/MaxObjectsCountFilters.h"


/** Filter checking for the existence of at most the given number of valid electrons.
*  Required config tag: MaxNElectrons
*/
std::string MaxElectronsCountFilter::GetFilterId() const
{
	return "MaxElectronsCountFilter";
}

void MaxElectronsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
				return product.m_validElectrons.size();
			},
			CutRange::UpperThresholdCut(double(settings.GetMaxNElectrons()))
	));
}


/** Filter checking for the existence of at most the given number of valid muons.
*  Required config tag: MaxNMuons
*/
std::string MaxMuonsCountFilter::GetFilterId() const
{
	return "MaxMuonsCountFilter";
}

void MaxMuonsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
				return product.m_validMuons.size();
			},
			CutRange::UpperThresholdCut(double(settings.GetMaxNMuons()))
	));
}


/** Filter checking for the existence of at most the given number of valid taus.
*  Required config tag: MaxNTaus
*/
std::string MaxTausCountFilter::GetFilterId() const
{
	return "MaxTausCountFilter";
}

void MaxTausCountFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
				return product.m_validTaus.size();
			},
			CutRange::UpperThresholdCut(double(settings.GetMaxNTaus()))
	));
}


/** Filter checking for the existence of at most the given number of valid jets.
*  Required config tag: MaxNJets
*/
std::string MaxJetsCountFilter::GetFilterId() const
{
	return "MaxJetsCountFilter";
}

void MaxJetsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
				return product.m_validJets.size();
			},
			CutRange::UpperThresholdCut(double(settings.GetMaxNJets()))
	));
}


/** Filter checking for the existence of at most the given number of valid b-tagged jets.
*  Required config tag: MaxNBTaggedJets
*/
std::string MaxBTaggedJetsCountFilter::GetFilterId() const
{
	return "MaxBTaggedJetsCountFilter";
}

void MaxBTaggedJetsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
				return product.m_bTaggedJets.size();
			},
			CutRange::UpperThresholdCut(double(settings.GetMaxNBTaggedJets()))
	));
}



/** Filter checking for the existence of at most the given number of valid b-tagged jets.
*  Required config tag: MaxNNonBTaggedJets
*/
std::string MaxNonBTaggedJetsCountFilter::GetFilterId() const
{
	return "MaxNonBTaggedJetsCountFilter";
}

void MaxNonBTaggedJetsCountFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata) {
				return product.m_nonBTaggedJets.size();
			},
			CutRange::UpperThresholdCut(double(settings.GetMaxNNonBTaggedJets()))
	));
}
