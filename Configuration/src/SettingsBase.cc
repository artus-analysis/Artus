
#include "Artus/Configuration/interface/ArtusConfig.h"
#include "Artus/Configuration/interface/SettingsBase.h"

SettingsBase::SettingsBase() :
	m_RootOutFile(nullptr)
{
}

SettingsBase::SettingsBase(std::string const& name) :
	m_Name(name),
	m_RootOutFile(nullptr)
{
}

SettingsBase::~SettingsBase() {
}

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


std::vector<SettingsBase::PipelineInfo> SettingsBase::GetPipelineInfos () const {

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

std::string SettingsBase::GetRootFileFolder() const {
	return GetName();
}

std::string SettingsBase::GetPipelinePrefix() const {
	if ( GetName() == "")
		return "";
	else
		return "Pipelines." + GetName() + ".";
}

std::string SettingsBase::ToString() const {
	return "SettingsBase - Pipeline name: " + GetName();
}

stringvector& SettingsBase::GetGlobalProcessors () const {
	RETURN_CACHED(m_globalProcessors, PropertyTreeSupport::GetAsStringList(GetPropTree(), "Processors" ))
}

std::vector<std::string> SettingsBase::GetAllProcessors () const {
	std::vector<std::string> allProcessors;
	std::vector<std::string> globalProcessors = GetGlobalProcessors();
	std::vector<std::string> localProcessors = GetProcessors();

	if ( GetName() != "") {
		allProcessors.insert(allProcessors.end(), globalProcessors.begin(), globalProcessors.end());
	}
	allProcessors.insert(allProcessors.end(), localProcessors.begin(), localProcessors.end());
	return allProcessors;
}

stringvector SettingsBase::GetFilters () const {
	return SettingsUtil::ExtractFilters(GetProcessors());
}
