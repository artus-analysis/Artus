// no code

#include <iostream>
#include <cstdlib>

#include "Artus/Configuration/interface/ArtusConfig.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"

ArtusConfig::ArtusConfig(int argc, char** argv) {

	std::cout << "This is closure MASS" << std::endl;

	if (argc < 2) {
		std::cerr << "Usage: " << argv[0]
				<< " json_config_file.json [VerboseLevel]\n";
		exit(1);
	}

	std::string jsonConfig = argv[1];
	boost::property_tree::json_parser::read_json(jsonConfig, m_propTreeRoot);

	m_outputPath = m_propTreeRoot.get < std::string > ("OutputPath");
	//std::string sLogFileName = g_sOutputPath + ".log";
	//g_logFile = new std::ofstream(sLogFileName.c_str(), std::ios_base::trunc);

	// input files

	// hast GC the file list ?
	/*
	 * for GC support, disable right now
	 * char* pPath;
	 pPath = getenv("FILE_NAMES");
	 if (pPath != NULL) {
	 boost::split(g_sourcefiles, pPath, boost::is_any_of(" "),
	 boost::token_compress_on);
	 // SplitVec == { "hello abc","ABC","aBc goodbye" }
	 } else {*/

	m_fileNames = PropertyTreeSupport::GetAsStringList(&m_propTreeRoot,
			"InputFiles");
	CALIB_LOG_FILE("Loading " << m_fileNames.size() << " input files.")

	if (m_fileNames.size() == 0) {
		CALIB_LOG_FATAL("No Kappa input files specified");
	}

	//BOOST_FOREACH(std::string s, g_sourcefiles)
	//{
	//    CALIB_LOG_FILE("Input File " << s)
	//}
}

