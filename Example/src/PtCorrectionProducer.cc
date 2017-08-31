
#include "Artus/Example/interface/PtCorrectionProducer.h"


std::string PtCorrectionProducer::GetProducerId() const
{
	return "pt_correction";
}

void PtCorrectionProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	ProducerBase<TraxTypes>::Init(settings, metadata);
}

void PtCorrectionProducer::Produce(event_type const& event, product_type & product,
		setting_type const& settings, metadata_type const& metadata) const
{

	product.m_floatPtSim_corrected = event.m_floatPtSim * settings.GetProducerPtCorrectionFactor();
}
