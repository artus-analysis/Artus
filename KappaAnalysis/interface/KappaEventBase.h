
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/Core/interface/EventBase.h"

/**
   \brief Defines the minimal content of the kappa ntuple.

   Defines the objects which are contained in a kappa ntuple. Members are pointer to the corresponding 
   collections of objects in the input file. This class contains pointers to the eventMetadata and 
   genEventMetadata, the minimal content of a kappa ntuple. 
*/

class KappaEventBase : public EventBase {
public:

	/// pointer to eventMetadata
	KEventInfo* m_eventInfo = 0;
	/// pointer to genEventmetaData
	KGenEventInfo* m_genEventInfo = 0;
};


