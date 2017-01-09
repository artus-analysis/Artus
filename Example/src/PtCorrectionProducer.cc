
#include "Artus/Example/interface/PtCorrectionProducer.h"


std::string PtCorrectionProducer::GetProducerId() const
{
	return "pt_correction";
}

void PtCorrectionProducer::Init(TraxSettings const& globalSettings)
{
	ProducerBase<TraxTypes>::Init(globalSettings);
}

void PtCorrectionProducer::Produce(TraxEvent const& event, TraxProduct & product,
		TraxSettings const& globalSettings) const
{

	product.m_floatPtSim_corrected = event.m_floatPtSim * globalSettings.GetProducerPtCorrectionFactor();
}
