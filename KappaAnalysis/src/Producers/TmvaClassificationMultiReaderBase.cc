
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationMultiReaderBase.h"


std::string MultiTmvaClassificationReader::GetProducerId() const
{
	return "MultiTmvaClassificationReader";
}

MultiTmvaClassificationReader::MultiTmvaClassificationReader() :
	TmvaClassificationMultiReaderBase(&setting_type::GetTmvaInputQuantities,
										&setting_type::GetTmvaMethods,
										&setting_type::GetTmvaWeights,
										&product_type::m_discriminators)
{
}