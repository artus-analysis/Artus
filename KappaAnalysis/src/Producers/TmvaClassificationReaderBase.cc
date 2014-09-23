
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationReaderBase.h"


std::string GeneralTmvaClassificationReader::GetProducerId() const
{
	return "GeneralTmvaClassificationReader";
}

GeneralTmvaClassificationReader::GeneralTmvaClassificationReader() :
	TmvaClassificationReaderBase(&KappaSettings::GetTmvaInputQuantities,
	                             &KappaSettings::GetTmvaMethods,
	                             &KappaSettings::GetTmvaWeights,
	                             &KappaProduct::m_discriminators)
{
}

