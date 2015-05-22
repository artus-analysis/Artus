#include "Artus/KappaAnalysis/interface/Filters/ObjectsCountFilters.h"

/** Filter checking for the existence of exactly the given number of valid electrons.
 *  Required config tag: NElectrons
 */
	std::string ElectronsCountFilter::GetFilterId() const {
		return "ElectronsCountFilter";
	}

	void ElectronsCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::EqualsCut(double(settings.GetNElectrons()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid muons.
 *  Required config tag: NMuons
 */
	std::string MuonsCountFilter::GetFilterId() const {
		return "MuonsCountFilter";
	}

	void MuonsCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validMuons.size();
				},
				CutRange::EqualsCut(double(settings.GetNMuons()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid taus.
 *  Required config tag: NTaus
 */
	std::string TausCountFilter::GetFilterId() const {
		return "TausCountFilter";
	}

	void TausCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validTaus.size();
				},
				CutRange::EqualsCut(double(settings.GetNTaus()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid jets.
 *  Required config tag: NJets
 */
	std::string JetsCountFilter::GetFilterId() const {
		return "JetsCountFilter";
	}

	void JetsCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validJets.size();
				},
				CutRange::EqualsCut(double(settings.GetNJets()))
		));
	}


/** Filter checking for the existence of exactly the given number of valid b-tagged jets.
 *  Required config tag: NBTaggedJets
 */
	std::string BTaggedJetsCountFilter::GetFilterId() const {
		return "BTaggedJetsCountFilter";
	}

	void BTaggedJetsCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_bTaggedJets.size();
				},
				CutRange::EqualsCut(double(settings.GetNBTaggedJets()))
		));
	}

