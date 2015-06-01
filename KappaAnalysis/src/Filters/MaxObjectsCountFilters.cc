#include "Artus/KappaAnalysis/interface/Filters/MaxObjectsCountFilters.h"


/** Filter checking for the existence of at most the given number of valid electrons.
 *  Required config tag: MaxNElectrons
 */
	std::string MaxElectronsCountFilter::GetFilterId() const {
		return "MaxElectronsCountFilter";
	}

	void MaxElectronsCountFilter::Init(KappaSettings const& settings) {

		FilterBase<KappaTypes>::Init(settings);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNElectrons()))
		));
	}


/** Filter checking for the existence of at most the given number of valid muons.
 *  Required config tag: MaxNMuons
 */
	std::string MaxMuonsCountFilter::GetFilterId() const {
		return "MaxMuonsCountFilter";
	}

	void MaxMuonsCountFilter::Init(KappaSettings const& settings) {

		FilterBase<KappaTypes>::Init(settings);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validMuons.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNMuons()))
		));
	}


/** Filter checking for the existence of at most the given number of valid taus.
 *  Required config tag: MaxNTaus
 */
	std::string MaxTausCountFilter::GetFilterId() const {
		return "MaxTausCountFilter";
	}

	void MaxTausCountFilter::Init(KappaSettings const& settings) {

		FilterBase<KappaTypes>::Init(settings);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validTaus.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNTaus()))
		));
	}


/** Filter checking for the existence of at most the given number of valid jets.
 *  Required config tag: MaxNJets
 */
	std::string MaxJetsCountFilter::GetFilterId() const {
		return "MaxJetsCountFilter";
	}

	void MaxJetsCountFilter::Init(KappaSettings const& settings) {

		FilterBase<KappaTypes>::Init(settings);

		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validJets.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNJets()))
		));
	}


/** Filter checking for the existence of at most the given number of valid b-tagged jets.
 *  Required config tag: MaxNBTaggedJets
 */
	std::string MaxBTaggedJetsCountFilter::GetFilterId() const {
		return "MaxBTaggedJetsCountFilter";
	}

	void MaxBTaggedJetsCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_bTaggedJets.size();
				},
				CutRange::UpperThresholdCut(double(settings.GetMaxNBTaggedJets()))
		));
	}

