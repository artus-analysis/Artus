
#include "Artus/KappaAnalysis/interface/Producers/NicknameProducer.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


std::string NicknameProducer::GetProducerId() const {
	return "NicknameProducer";
}

void NicknameProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	KappaProducerBase::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddStringQuantity("nickname", [](event_type const& event, product_type const& product)
	{
		return product.m_nickname;
	});
}

void NicknameProducer::Produce(event_type const& event, product_type& product,
                               setting_type const& settings, metadata_type const& metadata) const
{
	product.m_nickname = settings.GetNickname();
}

