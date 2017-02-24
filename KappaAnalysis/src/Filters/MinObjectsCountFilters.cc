#include "Artus/KappaAnalysis/interface/Filters/MinObjectsCountFilters.h"


/** Filter checking for the existence of at least the given number of valid electrons.
 *  Required config tag: MinNElectrons
 */
void MinElectronsCountFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validElectrons.size();
			},
			CutRange::LowerThresholdCut(double(settings.GetMinNElectrons()))
	));
}


/** Filter checking for the existence of at least the given number of valid muons.
 *  Required config tag: MinNMuons
 */
void MinMuonsCountFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validMuons.size();
			},
			CutRange::LowerThresholdCut(double(settings.GetMinNMuons()))
	));
}


/** Filter checking for the existence of at least the given number of valid taus.
 *  Required config tag: MinNTaus
 */
void MinTausCountFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validTaus.size();
			},
			CutRange::LowerThresholdCut(double(settings.GetMinNTaus()))
	));
}


/** Filter checking for the existence of at least the given number of valid jets.
 *  Required config tag: MinNJets
 */
void MinJetsCountFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validJets.size();
			},
			CutRange::LowerThresholdCut(double(settings.GetMinNJets()))
	));
}


/** Filter checking for the existence of at least the given number of valid b-tagged jets.
 *  Required config tag: MinNBTaggedJets
 */
void MinBTaggedJetsCountFilter::Init(KappaSettings const& settings) {
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_bTaggedJets.size();
			},
			CutRange::LowerThresholdCut(double(settings.GetMinNBTaggedJets()))
	));
}


/** Filter checking for the existence of at least the given number of valid b-tagged jets.
 *  Required config tag: MinNNonBTaggedJets
 */
void MinNonBTaggedJetsCountFilter::Init(KappaSettings const& settings) {
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_nonBTaggedJets.size();
			},
			CutRange::LowerThresholdCut(double(settings.GetMinNNonBTaggedJets()))
	));
}

