
#pragma once

#include <TH1.h>

#include "Artus/Core/interface/ConsumerBase.h"

#include "Artus/KappaAnalysisExample/interface/KappaExampleTypes.h"


class DiMuonSystemConsumer: public ConsumerBase<KappaExampleTypes> {
public:

	typedef typename KappaExampleTypes::event_type event_type;
	typedef typename KappaExampleTypes::product_type product_type;
	typedef typename KappaExampleTypes::setting_type setting_type;

	virtual std::string GetConsumerId() const override;

	virtual void Init(setting_type const& settings) override;

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                                  setting_type const& settings) override;

	virtual void Finish(setting_type const& settings) override;


private:
	mutable TH1* m_diMuonMassHistogram = 0;

};


