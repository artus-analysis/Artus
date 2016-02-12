
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


std::string GenElectronsFilter::GetFilterId() const {
	return "GenElectronsFilter";
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


std::string GenMuonsFilter::GetFilterId() const {
	return "GenMuonsFilter";
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


std::string GenTausFilter::GetFilterId() const {
	return "GenTausFilter";
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


std::string GenTauJetsFilter::GetFilterId() const {
	return "GenTauJetsFilter";
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
