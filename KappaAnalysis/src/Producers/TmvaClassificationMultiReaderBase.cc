
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationMultiReaderBase.h"


std::string MultiTmvaClassificationReader::GetProducerId() const
{
	return "MultiTmvaClassificationReader";
}

MultiTmvaClassificationReader::MultiTmvaClassificationReader() :
	TmvaClassificationMultiReaderBase(&KappaSettings::GetTmvaInputQuantities,
										&KappaSettings::GetTmvaMethods,
										&KappaSettings::GetTmvaWeights,
										&KappaProduct::m_discriminators)
{
}