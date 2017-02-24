
#include "Artus/KappaAnalysis/interface/Filters/ValidObjectsFilters.h"


void ValidElectronsFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validElectrons.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}


void ValidMuonsFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validMuons.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}


void ValidTausFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validTaus.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}


void ValidJetsFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validJets.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}


void ValidBTaggedJetsFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_bTaggedJets.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}


void GenElectronsFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_genElectrons.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}


void GenMuonsFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_genMuons.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}


void GenTausFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_genTaus.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}


void GenTauJetsFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_genTauJets.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}
