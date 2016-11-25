#include "Artus/KappaAnalysis/interface/Filters/ObjectsCountFilters.h"

/** Filter checking for the existence of exactly the given number of valid electrons.
 *  Required config tag: NElectrons
 */
void ElectronsCountFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

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
void MuonsCountFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

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
void TausCountFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

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
void JetsCountFilter::Init(KappaSettings const& settings) {

	FilterBase<KappaTypes>::Init(settings);

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
void BTaggedJetsCountFilter::Init(KappaSettings const& settings) {
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_bTaggedJets.size();
			},
			CutRange::EqualsCut(double(settings.GetNBTaggedJets()))
	));
}


/** Filter checking for the existence of exactly the given number of valid non b-tagged jets.
 *  Required config tag: NNonBTaggedJets
 */
void NonBTaggedJetsCountFilter::Init(KappaSettings const& settings) {
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_nonBTaggedJets.size();
			},
			CutRange::EqualsCut(double(settings.GetNNonBTaggedJets()))
	));
}

