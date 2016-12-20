
#include "Artus/Example/interface/TraxEventProvider.h"


TraxEventProvider::TraxEventProvider(std::vector<std::string> const& fileNames) :
		RootEventProvider<TraxTypes>(fileNames, "ec") // hardcode the root treename already here
{
}

void TraxEventProvider::WireEvent(TraxSettings const& settings)
{
	// set up the ROOT pointers to our local memory regions
	m_rootChain->SetBranchAddress("theSim", &m_event.m_floatTheSim);
	m_rootChain->SetBranchAddress("pSim", &m_event.m_floatPSim);
	m_rootChain->SetBranchAddress("ptSim", &m_event.m_floatPtSim);
	m_rootChain->SetBranchAddress("pzSim", &m_event.m_floatPzSim);
}

