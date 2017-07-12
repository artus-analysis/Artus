#pragma once

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/KappaAnalysis/interface/Utility/GenParticleDecayTreePrinter.h"



class PrintGenParticleDecayTreeConsumer: public ConsumerBase<KappaTypes>
{

public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;

	PrintGenParticleDecayTreeConsumer();

	std::string GetConsumerId() const override;
	
	virtual void Init(setting_type const& settings);

	void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                          setting_type const& settings) override;

	void Finish(setting_type const& settings) override;

private:
	GenParticleDecayTreePrinter genParticleDecayTreePrinter;
	KappaEnumTypes::GenCollectionToPrint genCollectionToPrint = KappaEnumTypes::GenCollectionToPrint::NONE;

};

