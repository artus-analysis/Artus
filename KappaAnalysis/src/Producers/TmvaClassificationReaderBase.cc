
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationReaderBase.h"


GeneralTmvaClassificationReader::GeneralTmvaClassificationReader() :
	TmvaClassificationReaderBase(&KappaSettings::GetTmvaInputQuantities,
	                             &KappaSettings::GetTmvaMethods,
	                             &KappaSettings::GetTmvaWeights,
	                             &KappaProduct::m_discriminators)
{
}

