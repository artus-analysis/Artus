
#include "Artus/KappaAnalysis/interface/Producers/NicknameProducer.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


void NicknameProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddStringQuantity("nickname", [](KappaEvent const& event, KappaProduct const& product)
	{
		return product.m_nickname;
	});
}

void NicknameProducer::Produce(KappaEvent const& event, KappaProduct& product,
                               KappaSettings const& settings) const
{
	product.m_nickname = settings.GetNickname();
}

