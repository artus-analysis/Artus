#include "Artus/KappaAnalysis/interface/Producers/PFCandidatesProducer.h"

void PFCandidatesProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	KappaProducerBase::Init(settings, metadata);
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "NPFChargedHadrons", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return product.m_pfChargedHadrons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "NPFNeutralHadrons", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return product.m_pfNeutralHadrons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "NPFElectrons", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return product.m_pfElectrons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "NPFMuons", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return product.m_pfMuons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "NPFPhotons", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return product.m_pfPhotons.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "NPFHadronicHF", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return product.m_pfHadronicHF.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "NPFElectromagneticHF", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return product.m_pfElectromagneticHF.size();
	});
}

void PFCandidatesProducer::Produce(event_type const& event, product_type& product, setting_type const& settings, metadata_type const& metadata) const
{
	for (KPFCandidates::const_iterator pfCandidate = event.m_packedPFCandidates->begin();
		pfCandidate != event.m_packedPFCandidates->end(); ++pfCandidate)
	{
		if (std::abs(pfCandidate->pdgId) == 211)
			fill_pfCandidate(product.m_pfChargedHadrons, product.m_pfChargedHadronsFromFirstPV,
				product.m_pfChargedHadronsNotFromFirstPV, &(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 130)
			fill_pfCandidate(product.m_pfNeutralHadrons, product.m_pfNeutralHadronsFromFirstPV,
				product.m_pfNeutralHadronsNotFromFirstPV, &(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 11)
			product.m_pfElectrons.push_back(&(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 13)
			product.m_pfMuons.push_back(&(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 22)
			fill_pfCandidate(product.m_pfPhotons, product.m_pfPhotonsFromFirstPV,
				product.m_pfPhotonsNotFromFirstPV, &(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 1)
			product.m_pfHadronicHF.push_back(&(*pfCandidate));
		else if (std::abs(pfCandidate->pdgId) == 2)
			product.m_pfElectromagneticHF.push_back(&(*pfCandidate));
		else
			std::cout << "Unknown PFCandidate!!!! pdgId: " << pfCandidate->pdgId << std::endl;
	}
}


void PFCandidatesProducer::fill_pfCandidate(std::vector<const KPFCandidate*>& full, std::vector<const KPFCandidate*>& fromFirstPV, std::vector<const KPFCandidate*>& notFromFirstPV, const KPFCandidate* currentCandidate) const
{
	full.push_back(currentCandidate);
	if (currentCandidate->fromFirstPVFlag > 1) fromFirstPV.push_back(currentCandidate);
	else notFromFirstPV.push_back(currentCandidate);
}
