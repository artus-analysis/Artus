
#include "Artus/Configuration/interface/ArtusConfig.h"
#include "Artus/Configuration/interface/SettingsBase.h"


stringvector SettingsUtil::ExtractFilters ( stringvector const& allProcessors )   {
	stringvector filt;

	for ( stringvector::const_iterator it = allProcessors.begin();
			it != allProcessors.end(); it ++ ) {
		const ArtusConfig::NodeTypePair nodeRes = ArtusConfig::ParseProcessNode( *it );
		if ( nodeRes.first == ProcessNodeType::Filter ){
			filt.push_back( nodeRes.second );
		}
	}

	return filt;
}
