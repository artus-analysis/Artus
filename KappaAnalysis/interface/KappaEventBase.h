
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

/**
   \brief Defines the minimal content of the kappa ntuple.

   Defines the objects which are contained in a kappa ntuple. Members are pointer to the corresponding 
   collections of objects in the input file. This class contains pointers to the eventMetadata and 
   genEventMetadata, the minimal content of a kappa ntuple. 
*/

class KappaEventBase {
public:

	/// pointer to eventMetadata
	KEventMetadata* m_eventMetadata = 0;
	/// pointer to genEventmetaData
	KGenEventMetadata* m_genEventMetadata = 0;
};


