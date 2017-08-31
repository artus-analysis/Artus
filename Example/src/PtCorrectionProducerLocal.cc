
#include "Artus/Example/interface/PtCorrectionProducerLocal.h"


std::string PtCorrectionProducerLocal::GetProducerId() const
{
	return "pt_correction_local";
}

void PtCorrectionProducerLocal::Init(setting_type const& settings, metadata_type& metadata) 
{
	ProducerBase<TraxTypes>::Init(settings, metadata);
}

void PtCorrectionProducerLocal::Produce(event_type const& event, product_type & product,
		setting_type const& settings, metadata_type const& metadata) const
{
	// m_floatPtSim_corrected has been set by the global producer before
	product.m_floatPtSim_corrected = product.m_floatPtSim_corrected * settings.GetProducerPtCorrectionFactorLocal();
}

