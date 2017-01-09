
#include "Artus/Example/interface/PtCorrectionProducerLocal.h"


std::string PtCorrectionProducerLocal::GetProducerId() const
{
	return "pt_correction_local";
}

void PtCorrectionProducerLocal::Init(setting_type const& settings) 
{
	ProducerBase<TraxTypes>::Init(settings);
}

void PtCorrectionProducerLocal::Produce(TraxEvent const& event,
		TraxProduct & product,
		TraxSettings const& localSettings) const
{
	// m_floatPtSim_corrected has been set by the global producer before
	product.m_floatPtSim_corrected = product.m_floatPtSim_corrected * localSettings.GetProducerPtCorrectionFactorLocal();
}

