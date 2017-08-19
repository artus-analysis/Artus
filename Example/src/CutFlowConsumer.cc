
#include "Artus/Example/interface/CutFlowConsumer.h"


void CutFlowConsumer::Finish(setting_type const& settings, metadata_type const& metadata)
{
	LOG(INFO) << "Cut Flow for pipeline" << m_pipelineName << ":";
	LOG(INFO) << m_flow.ToString();
}

