
#include <iostream>
#include <cstdlib>

#include <boost/program_options.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "TObjString.h"

#include "Artus/Configuration/interface/ArtusConfig.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Utility/interface/Utility.h"

ArtusConfig::ArtusConfig(int argc, char** argv) :
	m_jsonConfigFileName(""),
	m_minimumLogLevelString("")
{
	boost::program_options::options_description programOptions("Options");
	programOptions.add_options()
		("help,h", "Print help message")
		("log-level", boost::program_options::value< std::string >(&m_minimumLogLevelString),
		 "Detail level of logging (debug, info, warning, error, critical). [Default: taken from JSON config or info]")
		("json-config", boost::program_options::value< std::string >(&m_jsonConfigFileName),
		 "JSON config file");

	
	boost::program_options::positional_options_description positionalProgramOptions;
	positionalProgramOptions.add("json-config", 1);
	
	boost::program_options::variables_map optionsVariablesMap;
	boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(programOptions).positional(positionalProgramOptions).run(),
	                            optionsVariablesMap);
	boost::program_options::notify(optionsVariablesMap);
	
	// print help message, either if requested or if no parameters 
    // have been supplied
	if(optionsVariablesMap.count("help") || ( optionsVariablesMap.size() == 0 )) {
		std::cout << "Usage: " << argv[0] << " [options] JSON-CONFIG" << std::endl;
		std::cout << programOptions << std::endl;
		exit(0);
	}
	
	InitConfig();
	// logging can be uses from here on
}

std::pair<bool, el::Level> ArtusConfig::parseLogLevel(
		std::string const& minimumLogLevelString) const
{
	if (minimumLogLevelString == "debug")
	{
		return std::make_pair(true, el::Level::Debug);
	}
	else if (minimumLogLevelString == "info")
	{
		return std::make_pair(true, el::Level::Info);
	}
	else if (minimumLogLevelString == "warning")
	{
		return std::make_pair(true, el::Level::Warning);
	}
	else if (minimumLogLevelString == "error")
	{
		return std::make_pair(true, el::Level::Error);
	}
	else if (minimumLogLevelString == "critical")
	{
		return std::make_pair(true, el::Level::Fatal);
	}

	return std::make_pair(false, el::Level::Fatal);
}

ArtusConfig::ArtusConfig(std::stringstream & sStream)
{
	boost::property_tree::json_parser::read_json(sStream, m_propTreeRoot);

	InitConfig( true );
}

void ArtusConfig::InitConfig( bool configPreLoaded )
{
    // has the config been preloaded via the constructor already ?
    if (! configPreLoaded ) {
	    if(m_jsonConfigFileName.empty()) {
		    LOG(FATAL) << "NO JSON config specified!";
	    }
	
	    std::cout << "Loading Config file from \"" << m_jsonConfigFileName << "\"." << std::endl;
	    boost::property_tree::json_parser::read_json(m_jsonConfigFileName, m_propTreeRoot);
    }
	
	// init logging
	if(m_minimumLogLevelString.empty()) {
		m_minimumLogLevelString = m_propTreeRoot.get<std::string>("LogLevel", "info");
	}
	std::pair<bool, el::Level> minimumLogLevel = parseLogLevel(m_minimumLogLevelString);
	
	el::Configurations defaultLoggingConfig;
	defaultLoggingConfig.setToDefault();

	defaultLoggingConfig.set(el::Level::Debug, el::ConfigurationType::Enabled, "true");
	defaultLoggingConfig.set(el::Level::Info, el::ConfigurationType::Enabled, "true");
	defaultLoggingConfig.set(el::Level::Warning, el::ConfigurationType::Enabled, "true");
	defaultLoggingConfig.set(el::Level::Error, el::ConfigurationType::Enabled, "true");
	defaultLoggingConfig.set(el::Level::Fatal, el::ConfigurationType::Enabled, "true");
	
	if (minimumLogLevel.second <= el::Level::Info)
	{
		defaultLoggingConfig.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
	}
	if (minimumLogLevel.second <= el::Level::Warning)
	{
		defaultLoggingConfig.set(el::Level::Info, el::ConfigurationType::Enabled, "false");
	}
	if (minimumLogLevel.second <= el::Level::Error)
	{
		defaultLoggingConfig.set(el::Level::Warning, el::ConfigurationType::Enabled, "false");
	}
	if (minimumLogLevel.second <= el::Level::Fatal)
	{
		defaultLoggingConfig.set(el::Level::Error, el::ConfigurationType::Enabled, "false");
	}

	defaultLoggingConfig.set(el::Level::Debug, el::ConfigurationType::Format, "%msg");
	defaultLoggingConfig.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
	defaultLoggingConfig.set(el::Level::Warning, el::ConfigurationType::Format, "\033[0;30;43m%level:\033[0m %msg");
	defaultLoggingConfig.set(el::Level::Error, el::ConfigurationType::Format, "\033[0;37;41m%level:\033[0m %msg");
	defaultLoggingConfig.set(el::Level::Fatal, el::ConfigurationType::Format, "\033[0;37;41m%level:\033[0;31m %msg\033[0m");

	el::Loggers::reconfigureLogger("default", defaultLoggingConfig);

	m_outputPath = m_propTreeRoot.get<std::string>("OutputPath", "output.root");
	m_fileNames = PropertyTreeSupport::GetAsStringList(&m_propTreeRoot, "InputFiles");
	LOG(INFO) << "Loading " << m_fileNames.size() << " input files.";

	if (m_fileNames.size() == 0)
	{
		LOG(FATAL) << "No input files specified!";
	}
}

void ArtusConfig::SaveConfig(TFile * outputFile) const
{
	TObjString jsonConfigContent(
			Utility::ReadStringFromFile(m_jsonConfigFileName).c_str());
	outputFile->cd();
	jsonConfigContent.Write("config");
}

ArtusConfig::NodeTypePair ArtusConfig::ParseProcessNode(std::string const& sInp)
{
	std::vector < std::string > splitted;
	boost::algorithm::split(splitted, sInp, boost::algorithm::is_any_of(":"));
	transform(splitted.begin(), splitted.end(), splitted.begin(),
	          [](std::string s) { return boost::algorithm::trim_copy(s); });

	if (splitted.size() != 2)
	{
		LOG(FATAL) << "Process node description " << sInp << " cannot be parsed!";
	}

	ProcessNodeType ntype;

	if (splitted[0] == "filter")
	{
		ntype = ProcessNodeType::Filter;
	}
	else if (splitted[0] == "producer")
	{
		ntype = ProcessNodeType::Producer;
	}
	else
	{
		LOG(FATAL) << "process node type " << splitted[0] << " is unknown!";
	}

	return std::make_pair(ntype, splitted[1]);
}
