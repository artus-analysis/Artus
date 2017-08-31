
#include "Artus/KappaAnalysisExample/interface/DiMuonSystemConsumer.h"

#include "Artus/Utility/interface/RootFileHelper.h"


std::string DiMuonSystemConsumer::GetConsumerId() const
{
	return "DiMuonSystemConsumer";
}

void DiMuonSystemConsumer::Init(setting_type const& settings, metadata_type& metadata)
{
	ConsumerBase<KappaExampleTypes>::Init(settings, metadata);

	// create histogram after selecting the directory associated with the current pipeline
	RootFileHelper::SafeCd(settings.GetRootOutFile(),
	                       settings.GetRootFileFolder());
	
	m_diMuonMassHistogram = new TH1F("diMuonMass", "diMuonMass", 1000, 0.0, 1000.0);
}

void DiMuonSystemConsumer::ProcessFilteredEvent(event_type const& event, product_type const& product,
                                                setting_type const& settings, metadata_type const& metadata)
{
	// fill histogram
	m_diMuonMassHistogram->Fill(product.m_diMuonSystem.M());
}


void DiMuonSystemConsumer::Finish(setting_type const& settings, metadata_type const& metadata)
{
	// save histogram
	RootFileHelper::SafeCd(settings.GetRootOutFile(),
	                       settings.GetRootFileFolder());
	
	m_diMuonMassHistogram->Write(m_diMuonMassHistogram->GetName());
}

