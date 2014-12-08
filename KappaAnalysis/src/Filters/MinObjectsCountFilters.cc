#include "Artus/KappaAnalysis/interface/Filters/MinObjectsCountFilters.h"


/** Filter checking for the existance of at most the given number of valid electrons.
 *  Required config tag: MinNElectrons
 */
	std::string MinElectronsCountFilter::GetFilterId() const {
		return "MinElectronsCountFilter";
	}

	void MinElectronsCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validElectrons.size();
				},
				CutRange::LowerThresholdCut(double(settings.GetMinNElectrons()))
		));
	}


/** Filter checking for the existance of at most the given number of valid muons.
 *  Required config tag: MinNMuons
 */
	std::string MinMuonsCountFilter::GetFilterId() const {
		return "MinMuonsCountFilter";
	}

	void MinMuonsCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validMuons.size();
				},
				CutRange::LowerThresholdCut(double(settings.GetMinNMuons()))
		));
	}


/** Filter checking for the existance of at most the given number of valid taus.
 *  Required config tag: MinNTaus
 */
	std::string MinTausCountFilter::GetFilterId() const {
		return "MinTausCountFilter";
	}

	void MinTausCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validTaus.size();
				},
				CutRange::LowerThresholdCut(double(settings.GetMinNTaus()))
		));
	}


/** Filter checking for the existance of at most the given number of valid jets.
 *  Required config tag: MinNJets
 */
	std::string MinJetsCountFilter::GetFilterId() const {
		return "MinJetsCountFilter";
	}

	void MinJetsCountFilter::Init(KappaSettings const& settings) {
		this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
				[](KappaEvent const& event, KappaProduct const& product) {
					return product.m_validJets.size();
				},
				CutRange::LowerThresholdCut(double(settings.GetMinNJets()))
		));
	}
