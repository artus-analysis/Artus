
#include "Artus/Example/interface/CutFlowConsumer.h"


void CutFlowConsumer::Finish(setting_type const& setting)
{
	LOG(INFO) << "Cut Flow for pipeline" << m_pipelineName << ":";
	LOG(INFO) << m_flow.ToString();
}

