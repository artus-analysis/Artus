
#include "Artus/KappaAnalysis/interface/Producers/TmvaClassificationMultiReaderBase.h"


MultiTmvaClassificationReader::MultiTmvaClassificationReader() :
	TmvaClassificationMultiReaderBase(&KappaSettings::GetTmvaInputQuantities,
										&KappaSettings::GetTmvaMethods,
										&KappaSettings::GetTmvaWeights,
										&KappaProduct::m_discriminators)
{
}
