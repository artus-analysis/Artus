#include "Artus/KappaAnalysis/interface/Producers/PFCandidatesProducer.h"

void PFCandidatesProducer::Init(KappaSettings const& settings)
{
    KappaProducerBase::Init(settings);
    LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFChargedHadrons", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfChargedHadrons.size();
    });
    LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFNeutralHadrons", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfNeutralHadrons.size();
    });
    LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFElectrons", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfElectrons.size();
    });
    LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFMuons", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfMuons.size();
    });
    LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFPhotons", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfPhotons.size();
    });
    LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFHadronicHF", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfHadronicHF.size();
    });
    LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("NPFElectromagneticHF", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfElectromagneticHF.size();
    });
    LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("PFSumHt", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfSumHt;
    });
    LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("PFSumPt", [](KappaEvent const& event, KappaProduct const& product)
    {
        return product.m_pfSumP4.Pt();
    });
}

void PFCandidatesProducer::Produce(KappaEvent const& event, KappaProduct& product, KappaSettings const& settings) const
{
    product.m_pfSumP4.SetPxPyPzE(0.,0.,0.,0.);

    for (KPFCandidates::const_iterator pfCandidate = event.m_packedPFCandidates->begin();
        pfCandidate != event.m_packedPFCandidates->end(); ++pfCandidate)
    {
        if (std::abs(pfCandidate->pdgId) == 211)
            product.m_pfChargedHadrons.push_back(&(*pfCandidate));
        else if (std::abs(pfCandidate->pdgId) == 130)
            product.m_pfNeutralHadrons.push_back(&(*pfCandidate));
        else if (std::abs(pfCandidate->pdgId) == 11)
            product.m_pfElectrons.push_back(&(*pfCandidate));
        else if (std::abs(pfCandidate->pdgId) == 13)
            product.m_pfMuons.push_back(&(*pfCandidate));
        else if (std::abs(pfCandidate->pdgId) == 22)
            product.m_pfPhotons.push_back(&(*pfCandidate));
        else if (std::abs(pfCandidate->pdgId) == 1)
            product.m_pfHadronicHF.push_back(&(*pfCandidate));
        else if (std::abs(pfCandidate->pdgId) == 2)
            product.m_pfElectromagneticHF.push_back(&(*pfCandidate));
        else
            std::cout << "Unknown PFCandidate!!!! pdgId: " << pfCandidate->pdgId << std::endl;

        product.m_pfSumHt += pfCandidate->p4.Pt();
        product.m_pfSumP4 += pfCandidate->p4;
    }
}

