
#include "Artus/Example/interface/MeanPtConsumer.h"


MeanPtConsumer::MeanPtConsumer() :
		m_itemCount(0),
		m_mean(0.0f)
{

}

std::string MeanPtConsumer::GetConsumerId() const
{
	return "mean_pt";
}

void MeanPtConsumer::ProcessFilteredEvent(event_type const& event,
		product_type const& product, setting_type const& settings, metadata_type const& metadata)
{
	// ++i; m = (m * (i-1) + pt) / i = m + (pt - m) / i
	++m_itemCount;
	m_mean += (static_cast<double>(event.m_floatPtSim) - m_mean) / static_cast<double>(m_itemCount);
}

void MeanPtConsumer::Finish(setting_type const& settings, metadata_type const& metadata )
{
	// todo: print pipeline name here
	LOG(INFO) << "Track mean Pt is " << m_mean << " [GeV].";
}

