#pragma once

// only forward declared here
#include "TraxEventData.h"
#include "TraxMetaData.h"
#include "TraxMetaData.h"
#include "TraxPipelineSettings.h"

struct TraxTypes {
	typedef TraxEventData event_type;
	typedef TraxLocalMetaData local_meta_type;
	typedef TraxGlobalMetaData global_meta_type;
	typedef TraxPipelineSettings setting_type;
};
