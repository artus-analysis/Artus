
#include "Artus/KappaAnalysis/interface/Filters/ValidObjectsFilters.h"


std::string ValidElectronsFilter::GetFilterId() const {
	return "ValidElectronsFilter";
}

void ValidElectronsFilter::Init(KappaSettings const& settings) {
	CutRangeFilterBase::Init(settings);
	this->m_cuts.push_back(std::pair<double_extractor_lambda, CutRange>(
			[](KappaEvent const& event, KappaProduct const& product) {
				return product.m_validElectrons.size();
			},
			CutRange::LowerThresholdCut(1.0)
	));
}

std::string ValidMuonsFilter::GetFilterId() const {
	return "ValidMuonsFilter";
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

std::string ValidTausFilter::GetFilterId() const {
	return "ValidTausFilter";
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


std::string ValidJetsFilter::GetFilterId() const {
	return "ValidJetsFilter";
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


std::string ValidBTaggedJetsFilter::GetFilterId() const {
	return "ValidBTaggedJetsFilter";
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
