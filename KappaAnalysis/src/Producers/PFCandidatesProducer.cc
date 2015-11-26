
#include "boost/algorithm/string/join.hpp"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/Producers/PFCandidatesProducer.h"


std::string PFCandidatesProducer::GetProducerId() const {
	return "PFCandidatesProducer";
}

PFCandidatesProducer::~PFCandidatesProducer()
{
	LOG(DEBUG) << "Constructed event weight from indidual weights ("
	           << boost::algorithm::join(m_weightNames, ", ")
	           << ") in the pipeline \"" << pipelineName << "\".";
}

void PFCandidatesProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);
	pipelineName = settings.GetName();
}

void PFCandidatesProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                  KappaSettings const& settings) const
{
	assert(event.m_pfChargedHadronsNoPileUp);
	std::cout << "m_pfChargedHadronsNoPileUp size:" << event.m_pfChargedHadronsNoPileUp->size() << std::endl;
	for (typename std::vector<KPFCandidate>::iterator candidate = event.m_pfChargedHadronsNoPileUp->begin();
			candidate != event.m_pfChargedHadronsNoPileUp->end(); ++candidate)
	{
		std::cout << candidate->p4 << std::endl;
	}
	
	std::cout << "m_pfChargedHadronsPileUp size:" << event.m_pfChargedHadronsPileUp->size() << std::endl;
	for (typename std::vector<KPFCandidate>::iterator candidate = event.m_pfChargedHadronsPileUp->begin();
			candidate != event.m_pfChargedHadronsPileUp->end(); ++candidate)
	{
		std::cout << candidate->p4 << std::endl;
	}
	std::cout << "m_pfAllChargedParticlesNoPileUp size:" << event.m_pfAllChargedParticlesNoPileUp->size() << std::endl;
	for (typename std::vector<KPFCandidate>::iterator candidate = event.m_pfAllChargedParticlesNoPileUp->begin();
			candidate != event.m_pfAllChargedParticlesNoPileUp->end(); ++candidate)
	{
		std::cout << candidate->p4 << std::endl;
	}
	//m_pfAllChargedParticles
			
}


