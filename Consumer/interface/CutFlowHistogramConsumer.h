
#pragma once

//#include <boost/scoped_ptr.hpp>

#include <TH1.h>
#include "TROOT.h"

#include "Artus/Utility/interface/RootFileHelper.h"
#include "Artus/Consumer/interface/CutFlowConsumerBase.h"


template < class TTypes >
class CutFlowHistogramConsumer: public CutFlowConsumerBase< TTypes > {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef typename std::function<float(event_type const&, product_type const&, setting_type const&)> weight_extractor_lambda;

	std::string GetConsumerId() const override
	{
		return "cutflow_histogram";
	}

	CutFlowHistogramConsumer() :
		CutFlowConsumerBase< TTypes >(),
		m_addWeightedCutFlow(false),
		m_histogramsInitialised(false)
	{
	}

	void Init(typename TTypes::setting_type const& settings) override
	{
		CutFlowConsumerBase<TTypes>::Init(settings);
		// default weight = 1.0
		// overwrite this in analysis-specific code
		// and set m_addWeightedCutFlow to true
		weightExtractor = [](event_type const&, product_type const&, setting_type const&) { return 1.0; };
	}

	void ProcessEvent(event_type const& event,
	                  product_type const& product,
	                  setting_type const& setting,
	                  FilterResult & filterResult) override
	{
		CutFlowConsumerBase<TTypes>::ProcessEvent(event, product, setting, filterResult);

		// initialise histograms in first event
		if(! m_histogramsInitialised) {
			m_histogramsInitialised = InitialiseHistograms(setting, filterResult);
		}

		float weight = weightExtractor(event, product, setting);
		int bin = 0;

		// fill first bin of histograms with all events
		m_cutFlowUnweightedHist->Fill(static_cast<float>(bin));

		if(m_addWeightedCutFlow) {
			m_cutFlowWeightedHist->Fill(static_cast<float>(bin), weight);
		}

		// fill bins of histograms corresponding to passed filters
		FilterResult::FilterDecisions const& filterDecisions = filterResult.GetFilterDecisions();
		for(FilterResult::FilterDecisions::const_iterator filterDecision = filterDecisions.begin();
		    filterDecision != filterDecisions.end(); ++filterDecision)
		{
			++bin;
			if (filterDecision->filterDecision == FilterResult::Decision::Passed ||
			    filterDecision->taggingMode == FilterResult::TaggingMode::Tagging)
			{
				m_cutFlowUnweightedHist->Fill(static_cast<float>(bin));

				if(m_addWeightedCutFlow) {
					m_cutFlowWeightedHist->Fill(static_cast<float>(bin), weight);
				}
			}
		}
	}

	void Finish(setting_type const& setting) override {
		CutFlowConsumerBase<TTypes>::Finish(setting);
		
		if (m_histogramsInitialised)
		{
			// save histograms
			RootFileHelper::SafeCd(setting.GetRootOutFile(),
					setting.GetRootFileFolder());

			m_cutFlowUnweightedHist->Write(m_cutFlowUnweightedHist->GetName());

			if(m_addWeightedCutFlow) {
				m_cutFlowWeightedHist->Write(m_cutFlowWeightedHist->GetName());
			}
		}
	}

protected:
	TH1F* m_cutFlowUnweightedHist = nullptr;
	TH1F* m_cutFlowWeightedHist = nullptr;
	weight_extractor_lambda weightExtractor;
	bool m_addWeightedCutFlow;

private:
	bool m_histogramsInitialised;

	// initialise histograms; to be called in first event
	bool InitialiseHistograms( setting_type const& setting, FilterResult & filterResult) {

		// filters
		std::vector<std::string> filterNames = filterResult.GetFilterNames();
		int nFilters = filterNames.size();

		// histograms
		RootFileHelper::SafeCd( setting.GetRootOutFile(),
		                        setting.GetRootFileFolder());

		std::string cutFlowHistTitle("Cut Flow for Pipeline \"" + setting.GetName() + "\"");

		m_cutFlowUnweightedHist = new TH1F("cutFlowUnweighted",
		                                   cutFlowHistTitle.c_str(),
		                                   nFilters+1, 0.0, nFilters+1.0);

		if(m_addWeightedCutFlow) {
			m_cutFlowWeightedHist = new TH1F("cutFlowWeighted",
			                                 cutFlowHistTitle.c_str(),
			                                 nFilters+1, 0.0, nFilters+1.0);
		}

		// names for bins
		int bin = 1;

		m_cutFlowUnweightedHist->GetXaxis()->SetBinLabel(bin, "without filters");

		if(m_addWeightedCutFlow) {
			m_cutFlowWeightedHist->GetXaxis()->SetBinLabel(bin, "without filters");
		}

		for(std::vector<std::string>::const_iterator filterName = filterNames.begin();
		    filterName != filterNames.end(); ++filterName)
		{
			++bin;
			std::string filterNameLabel = *filterName;
			if (filterResult.IsTaggingFilter(filterNameLabel) == FilterResult::TaggingMode::Tagging) {
				filterNameLabel += " (T)";
			}
			m_cutFlowUnweightedHist->GetXaxis()->SetBinLabel(bin, filterNameLabel.c_str());

			if(m_addWeightedCutFlow) {
				m_cutFlowWeightedHist->GetXaxis()->SetBinLabel(bin, filterNameLabel.c_str());
			}
		}

		return true;
	}

};
