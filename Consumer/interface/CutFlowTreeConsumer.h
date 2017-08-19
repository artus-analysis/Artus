
#pragma once

//#include <boost/scoped_ptr.hpp>

#include <TDirectory.h>
#include <TTree.h>
#include <TROOT.h>

#include "Artus/Utility/interface/RootFileHelper.h"
#include "Artus/Consumer/interface/CutFlowConsumerBase.h"


template < class TTypes >
class CutFlowTreeConsumer: public CutFlowConsumerBase< TTypes > {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;
	
	typedef typename std::function<uint64_t(event_type const&, product_type const&, setting_type const&)> uint64_extractor_lambda;

	std::string GetConsumerId() const override
	{
		return "CutFlowTreeConsumer";
	}
	
	CutFlowTreeConsumer() :
		CutFlowConsumerBase< TTypes >(),
		m_treesInitialised(false)
	{
	}

	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		CutFlowConsumerBase<TTypes>::Init(settings, metadata);
		
		// default run,lumi,event = 1.0
		// overwrite this in analysis-specific code
		m_runExtractor = [](event_type const&, product_type const&, setting_type const&) { return 1.0; };
		m_lumiExtractor = [](event_type const&, product_type const&, setting_type const&) { return 1.0; };
		m_eventExtractor = [](event_type const&, product_type const&, setting_type const&) { return 1.0; };
	}

	void ProcessEvent(event_type const& event, product_type const& product,
	                  setting_type const& settings, metadata_type const& metadata,
	                  FilterResult & filterResult) override
	{
		CutFlowConsumerBase<TTypes>::ProcessEvent(event, product, settings, metadata, filterResult);
		
		// initialise histograms in first event
		if(! m_treesInitialised) {
			m_treesInitialised = InitialiseTrees(settings, filterResult);
		}
		
		m_run = m_runExtractor(event, product, settings);
		m_lumi = m_lumiExtractor(event, product, settings);
		m_event = m_eventExtractor(event, product, settings);
		
		// fill tree corresponding to non-passed filters
		size_t filterIndex = 0;
		FilterResult::FilterDecisions const& filterDecisions = filterResult.GetFilterDecisions();
		for(FilterResult::FilterDecisions::const_iterator filterDecision = filterDecisions.begin();
		    filterDecision != filterDecisions.end(); ++filterDecision)
		{
			if ((filterDecision->filterDecision != FilterResult::Decision::Passed) &&
			    (filterDecision->taggingMode == FilterResult::TaggingMode::Filtering)) {
				m_cutFlowTrees[filterIndex]->Fill();
				break;
			}
			
			++filterIndex;
		}
	}

	void Finish(setting_type const& settings, metadata_type const& metadata) override {
		CutFlowConsumerBase<TTypes>::Finish(settings, metadata);
		
		if (m_treesInitialised)
		{
			// save histograms
			RootFileHelper::SafeCd(settings.GetRootOutFile(),
					settings.GetRootFileFolder());
		
			for (std::vector<TTree*>::iterator cutFlowTree = m_cutFlowTrees.begin(); cutFlowTree != m_cutFlowTrees.end(); ++cutFlowTree)
			{
				(*cutFlowTree)->Write((*cutFlowTree)->GetName());
			}
		}
	}

protected:
	std::vector<TTree*> m_cutFlowTrees;
	uint64_extractor_lambda m_runExtractor;
	uint64_extractor_lambda m_lumiExtractor;
	uint64_extractor_lambda m_eventExtractor;

private:
	bool m_treesInitialised;
	uint64_t m_run;
	uint64_t m_lumi;
	uint64_t m_event;
	
	// initialise histograms; to be called in first event
	bool InitialiseTrees(setting_type const& settings, FilterResult & filterResult) {

		// filters
		std::vector<std::string> filterNames = filterResult.GetFilterNames();
	
		// trees
		TDirectory* tmpDirectory = gDirectory;
		RootFileHelper::SafeCd(settings.GetRootOutFile(),
		                       settings.GetRootFileFolder());
		
		for(size_t filterIndex = 0; filterIndex < filterNames.size(); ++filterIndex)
		{
			std::string name("discardedEvents" + std::to_string(filterIndex+1) + "_" + filterNames[filterIndex]);
			std::string title("Events discarded in Filter \"" + filterNames[filterIndex] + "\"");
			
			TTree* cutFlowTree = new TTree(name.c_str(), title.c_str());
			
			cutFlowTree->Branch("run", &m_run, "run/l");
			cutFlowTree->Branch("lumi", &m_run, "lumi/l");
			cutFlowTree->Branch("event", &m_run, "event/l");
			
			m_cutFlowTrees.push_back(cutFlowTree);
			
		}
		gDirectory = tmpDirectory;
		
		return true;
	}

};
