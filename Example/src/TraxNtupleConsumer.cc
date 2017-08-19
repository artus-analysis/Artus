
#include "Artus/Example/interface/TraxNtupleConsumer.h"


std::string TraxNtupleConsumer::GetConsumerId() const
{
	return "ntuple";
}

float TraxNtupleConsumer::returnvalue(std::string string, event_type const& event, product_type const& product)
{
	if (string == "pt") {
		return event.m_floatPtSim;
	} else if (string == "pt_corr") {
		return product.m_floatPtSim_corrected;
	} else if (string == "theta") {
		return event.m_floatTheSim;
	} else {
		LOG(FATAL)<< "The quantity " << string << " could not be added to the Ntuple!";
		// the log fatal call will stop the execution of the program
		// this is to satisfy the compiler with a return value
		return DefaultValues::UndefinedFloat;
	}
}
