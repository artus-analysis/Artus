
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/KappaAnalysis/interface/Utility/BtagSF.h"


/**
   \brief Producer for valid b-tagged jets.

   Exploits the properties of b-tagged jets
*/
class ValidBTaggedJetsProducer: public KappaProducerBase
{

public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;

	enum class BTagScaleFactorMethod : int
	{
		NONE = -1,
		PROMOTIONDEMOTION = 0,
		OTHER = 1,
	};
	static BTagScaleFactorMethod ToBTagScaleFactorMethod(std::string const& bTagSFMethod)
	{
		if (bTagSFMethod == "promotiondemotion") return BTagScaleFactorMethod::PROMOTIONDEMOTION;
		else if (bTagSFMethod == "other") return BTagScaleFactorMethod::OTHER;
		else return BTagScaleFactorMethod::NONE;
	}

protected:

	virtual bool AdditionalCriteria(KDataPFTaggedJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const;

private:

	BTagScaleFactorMethod bTagSFMethod;

};
