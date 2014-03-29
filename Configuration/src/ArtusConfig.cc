#include <iostream>
#include <cstdlib>

#include "TObjString.h"

#include "Artus/Configuration/interface/ArtusConfig.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Utility/interface/Utility.h"

ArtusConfig::ArtusConfig(int argc, char** argv)
{
	m_minimumLogLevel = el::Level::Info;
	// set defaults for logging
	el::Configurations defaultConf;
	defaultConf.setToDefault();

	defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "false");
	defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, "true");
	defaultConf.set(el::Level::Warning, el::ConfigurationType::Enabled, "true");
	defaultConf.set(el::Level::Error, el::ConfigurationType::Enabled, "true");
	defaultConf.set(el::Level::Fatal, el::ConfigurationType::Enabled, "true");

	el::Loggers::reconfigureLogger("default", defaultConf);

	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0]
				<< " json_config_file.json [VerboseLevel]\n";
		exit(1);
	}

	m_jsonConfigFileName = argv[1];
	std::cout << "Loading Config file from " << m_jsonConfigFileName
			<< std::endl;

	if (argc > 2)
	{
		std::string logLevelString(argv[2]);

		const size_t prefixSize(6); // the character count of "--log-"
		if (logLevelString.size() > prefixSize)
		{
			std::string lvlSubString = logLevelString.substr(prefixSize,
					std::string::npos);
			std::pair<bool, el::Level> parseRes = parseLogLevel(lvlSubString);
			if (parseRes.first)
			{
				m_minimumLogLevel = parseRes.second;
			}
		}
	}

	boost::property_tree::json_parser::read_json(m_jsonConfigFileName,
			m_propTreeRoot);

	InitConfig();
}

std::pair<bool, el::Level> ArtusConfig::parseLogLevel(
		std::string const& logLevelString) const
{
	if (logLevelString == "debug")
	{
		return std::make_pair(true, el::Level::Debug);
	}
	else if (logLevelString == "info")
	{
		return std::make_pair(true, el::Level::Info);
	}
	else if (logLevelString == "warning")
	{
		return std::make_pair(true, el::Level::Warning);
	}
	else if (logLevelString == "error")
	{
		return std::make_pair(true, el::Level::Error);
	}
	else if (logLevelString == "critical")
	{
		return std::make_pair(true, el::Level::Fatal);
	}

	return std::make_pair(false, el::Level::Fatal);
}

ArtusConfig::ArtusConfig(std::stringstream & sStream)
{
	boost::property_tree::json_parser::read_json(sStream, m_propTreeRoot);

	InitConfig();
}

void ArtusConfig::InitConfig()
{
	// is there a logging setting in the configuration file ?
	//if (  )

	el::Configurations defaultConf;
	defaultConf.setToDefault();

	defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "true");
	defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled, "true");
	defaultConf.set(el::Level::Warning, el::ConfigurationType::Enabled, "true");
	defaultConf.set(el::Level::Error, el::ConfigurationType::Enabled, "true");
	defaultConf.set(el::Level::Fatal, el::ConfigurationType::Enabled, "true");

	if (m_minimumLogLevel >= el::Level::Info)
		defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled,
				"false");
	if (m_minimumLogLevel >= el::Level::Warning)
		defaultConf.set(el::Level::Info, el::ConfigurationType::Enabled,
				"false");
	if (m_minimumLogLevel >= el::Level::Error)
		defaultConf.set(el::Level::Warning, el::ConfigurationType::Enabled,
				"false");
	if (m_minimumLogLevel >= el::Level::Fatal)
		defaultConf.set(el::Level::Error, el::ConfigurationType::Enabled,
				"false");

	defaultConf.set(el::Level::Debug, el::ConfigurationType::Format,
			"%level (%loc): %msg");
	defaultConf.set(el::Level::Info, el::ConfigurationType::Format, "%msg");
	defaultConf.set(el::Level::Warning, el::ConfigurationType::Format,
			"%level: %msg");
	defaultConf.set(el::Level::Error, el::ConfigurationType::Format,
			"%level: %msg");
	defaultConf.set(el::Level::Fatal, el::ConfigurationType::Format,
			"%level: %msg");

	el::Loggers::reconfigureLogger("default", defaultConf);

	m_outputPath = m_propTreeRoot.get < std::string > ("OutputPath");
	m_fileNames = PropertyTreeSupport::GetAsStringList(&m_propTreeRoot, "InputFiles");
	LOG(INFO) << "Loading " << m_fileNames.size() << " input files.";

	if (m_fileNames.size() == 0)
	{
		LOG(FATAL) << "No Kappa input files specified!";
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
