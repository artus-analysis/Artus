
#include "Artus/Configuration/interface/ArtusConfig.h"
#include "Artus/Configuration/interface/SettingsBase.h"


stringvector SettingsUtil::ExtractFilters ( stringvector const& allProcessors )   {
	stringvector filt;

	for ( stringvector::const_iterator it = allProcessors.begin();
			it != allProcessors.end(); ++it) {
		const ArtusConfig::NodeTypePair nodeRes = ArtusConfig::ParseProcessNode( *it );
		if ( nodeRes.first == ProcessNodeType::Filter ){
			filt.push_back( nodeRes.second );
		}
	}

	return filt;
}


std::vector <SettingsBase::PipelineInfo> SettingsBase::GetPipelineInfos () const {

	if ( ! m_pipelineInfos.IsCached() ) {
		std::vector <SettingsBase::PipelineInfo> pinfo;

		BOOST_FOREACH(boost::property_tree::ptree::value_type& v,
				GetPropTree()->get_child("Pipelines")) {

			SettingsBase pset;
			std::string sKeyName = v.first.data();

			pset.SetName(sKeyName);
			pset.SetPropTreePath("Pipelines." + sKeyName);
			pset.SetPropTree( GetPropTree() );

			pinfo.push_back( std::make_pair ( sKeyName, pset.GetLevel() ));
		}

		m_pipelineInfos.SetCache ( pinfo );
	}

	return m_pipelineInfos.GetValue();
}
