#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief Producer that offers writing out the nickname to the tree
   
   Required config tag:
   - Nickname (string)
*/
class NicknameProducer : public KappaProducerBase {
public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;

};
