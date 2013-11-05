#pragma once

// only forward declared here
class TraxEventData;
class TraxLocalMetaData;
class TraxGlobalMetaData;
class TraxPipelineSettings;

struct TraxTypes {
	typedef TraxEventData event_type;
	typedef TraxLocalMetaData local_meta_type;
	typedef TraxGlobalMetaData global_meta_type;
	typedef TraxPipelineSettings setting_type;
};
