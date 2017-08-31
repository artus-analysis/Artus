
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationReaderBase.h"


std::string GeneralTmvaClassificationReader::GetProducerId() const
{
	return "GeneralTmvaClassificationReader";
}

GeneralTmvaClassificationReader::GeneralTmvaClassificationReader() :
	TmvaClassificationReaderBase(&setting_type::GetTmvaInputQuantities,
	                             &setting_type::GetTmvaMethods,
	                             &setting_type::GetTmvaWeights,
	                             &product_type::m_discriminators)
{
}

