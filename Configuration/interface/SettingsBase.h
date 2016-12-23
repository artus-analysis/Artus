
#pragma once

#include "TFile.h"

#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Configuration/interface/SettingMacros.h"
#include "Artus/Configuration/interface/PropertyMacros.h"
#include "Artus/Configuration/interface/VarCache.h"

/**
   \brief Reads setting of local parts of PipelineRunner from a prepared json configuration file.

   Defines what is needed to read configurations for Artus event processing from a prepared json 
   file and passes it on to LocalProducers, Filters and Consumers.
*/

class SettingsUtil {
public:
	static std::vector<std::string> ExtractFilters ( std::vector<std::string> const& allProcessors );
};

class SettingsBase {
public:
	SettingsBase();

	explicit SettingsBase(std::string const& name);

	virtual ~SettingsBase();

	/// path in the config file to reach the settings for this pipeline
	IMPL_PROPERTY(std::string, PropTreePath)
	/// pointer to the global, loaded property tree
	IMPL_PROPERTY_INITIALIZE(boost::property_tree::ptree*, PropTree, nullptr)

	/// pipeline level, the default if no entry is in the json file will be 1
	IMPL_SETTING_DEFAULT(size_t, Level, 1 )

	IMPL_SETTING_DEFAULT(long long, FirstEvent, 0)
	IMPL_SETTING_DEFAULT(long long, ProcessNEvents, -1) // -1 for no limit

	IMPL_PROPERTY( std::string, Name )

	IMPL_SETTING_DEFAULT( std::string , LogLevel, "unknown" )

	/// Input file names
	IMPL_SETTING_STRINGLIST(InputFiles);

	/// the folder name in the output root file where plots or ntuples of this pipeline will end 
	/// up, if you want it not to be the pipeline name, override it
	virtual std::string GetRootFileFolder() const;

	std::string GetPipelinePrefix() const;

	/// a pointer to the root file where all the output will be stored must be set by the 
	/// application
	IMPL_PROPERTY(TFile *, RootOutFile)

	/// detemine whether this is data or MC
	IMPL_SETTING(bool, InputIsData);

	virtual std::string ToString() const;

	/// get list of all local producers
	IMPL_SETTING_STRINGLIST( Processors )
	IMPL_SETTING_STRINGLIST( Consumers )

	///
	//IMPL_GLOBAL_SETTING_STRINGLIST( GlobalProcessors )
	VarCache<std::vector<std::string>> m_globalProcessors;
	std::vector<std::string>& GetGlobalProcessors () const;

	std::vector<std::string> GetAllProcessors () const;

	// list of quantities needed for ntuple consumers
	//IMPL_SETTING_STRINGLIST(Quantities);
	IMPL_SETTING_SORTED_STRINGLIST(Quantities);

	virtual std::vector<std::string> GetFilters () const;

	IMPL_SETTING_STRINGLIST_DEFAULT(TaggingFilters, std::vector<std::string>());

	typedef std::pair < std::string, size_t > PipelineInfo;
	typedef std::vector<PipelineInfo> PipelineInfos;

	// returns the name and level of all pipelines
	// in the config file
	PipelineInfos GetPipelineInfos () const;

private:
	VarCache < PipelineInfos > m_pipelineInfos;

};

