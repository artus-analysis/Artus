#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief Producer that offers writing out the nickname to the tree
   
   Required config tag:
   - Nickname (string)
*/
class NicknameProducer : public KappaProducerBase {
public:

	virtual std::string GetProducerId() const override;

	virtual void Init(KappaSettings const& settings) override;

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};
