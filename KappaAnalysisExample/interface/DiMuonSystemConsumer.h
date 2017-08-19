
#pragma once

#include <TH1.h>

#include "Artus/Core/interface/ConsumerBase.h"

#include "Artus/KappaAnalysisExample/interface/KappaExampleTypes.h"


class DiMuonSystemConsumer: public ConsumerBase<KappaExampleTypes> {
public:

	virtual std::string GetConsumerId() const override;

	virtual void Init(setting_type const& settings, metadata_type& metadata) override;

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                                  setting_type const& settings, metadata_type const& metadata) override;

	virtual void Finish(setting_type const& settings, metadata_type const& metadata) override;


private:
	mutable TH1* m_diMuonMassHistogram = 0;

};


