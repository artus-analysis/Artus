#include "Artus/KappaAnalysis/interface/Producers/NumberOfParticlesProducer.h"

#include "Artus/KappaAnalysis/interface/Producers/ValidElectronsProducer.h"
#include "Artus/KappaAnalysis/interface/Producers/ValidMuonsProducer.h"

void NumberOfParticlesProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	KappaProducerBase::Init(settings, metadata);
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NLooseElectrons", [](event_type const& event, product_type const& product)
	{
		return product.m_NLooseElectrons;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NLooseElectronsRelaxedVtxCriteria", [](event_type const& event, product_type const& product)
	{
		return product.m_NLooseElectronsRelaxedVtxCriteria;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NEmbeddingMuons", [](event_type const& event, product_type const& product)
	{
		return product.m_NEmbeddingMuons;
	});
}

void NumberOfParticlesProducer::Produce(event_type const& event, product_type& product, setting_type const& settings, metadata_type const& metadata) const
{
	for (KElectrons::const_iterator electron = event.m_electrons->begin();
		electron != event.m_electrons->end(); ++electron)
	{
		if(ValidElectronsProducer<KappaTypes>::IsLooseVbtf95ElectronRelaxedVtxCriteria(&(*electron), event, product))
		{
			product.m_NLooseElectronsRelaxedVtxCriteria++;
			if(ValidElectronsProducer<KappaTypes>::IsLooseVbtf95Electron(&(*electron), event, product)) product.m_NLooseElectrons++;
		}
	}

	for (KMuons::const_iterator muon = event.m_muons->begin();
		muon != event.m_muons->end(); ++muon)
	{
		if(ValidMuonsProducer<KappaTypes>::IsEmbeddingMuon(&(*muon), event, product)) product.m_NEmbeddingMuons++;
	}
}
