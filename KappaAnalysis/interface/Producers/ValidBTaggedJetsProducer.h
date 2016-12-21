
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"
#include "Artus/KappaAnalysis/interface/Utility/BTagSF.h"


/**
   \brief Producer for valid b-tagged jets.

   Exploits the properties of b-tagged jets
*/
class ValidBTaggedJetsProducer: public KappaProducerBase
{

public:

	std::string GetProducerId() const override;

	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

	enum class BTagScaleFactorMethod : int
	{
		NONE = -1,
		PROMOTIONDEMOTION = 0,
		OTHER = 1,
	};
	static BTagScaleFactorMethod ToBTagScaleFactorMethod(std::string const& bTagSFMethod);

protected:

	virtual bool AdditionalCriteria(KJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const;

private:

	BTagScaleFactorMethod m_bTagSFMethod;
	std::map<std::string,float> m_bTagWorkingPoints;
	std::map<std::string,BTagSF*> m_bTagSfMap;

};
