/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#include <iostream>
#include <cstdlib>

#include "TObjString.h"

#include "Artus/Configuration/interface/ArtusConfig.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"
#include "Artus/Utility/interface/Utility.h"

ArtusConfig::ArtusConfig(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0]
				<< " json_config_file.json [VerboseLevel]\n";
		exit(1);
	}

	m_jsonConfigFileName = argv[1];
	std::cout << "Loading Config file from " << m_jsonConfigFileName << std::endl;
	boost::property_tree::json_parser::read_json(m_jsonConfigFileName, m_propTreeRoot);

	InitConfig ();
}

ArtusConfig::ArtusConfig( std::stringstream & sStream ) {
	boost::property_tree::json_parser::read_json(sStream, m_propTreeRoot);

	InitConfig();
}

void ArtusConfig::InitConfig() {

	m_outputPath = m_propTreeRoot.get < std::string > ("OutputPath");
	m_fileNames = PropertyTreeSupport::GetAsStringList(&m_propTreeRoot,
			"InputFiles");
	ARTUS_LOG_FILE("Loading " << m_fileNames.size() << " input files.")

	if (m_fileNames.size() == 0) {
		ARTUS_LOG_FATAL("No Kappa input files specified");
	}
}

void ArtusConfig::SaveConfig(TFile * outputFile) const {
	TObjString jsonConfigContent(Utility::ReadStringFromFile(m_jsonConfigFileName).c_str());
	outputFile->cd();
	jsonConfigContent.Write("config");
}


ArtusConfig::NodeTypePair ArtusConfig::ParseProcessNode ( std::string const& sInp ) {
	std::vector < std::string > splitted;
	boost::algorithm::split( splitted, sInp, boost::algorithm::is_any_of(":") );

	if ( splitted.size() != 2 ) {
		ARTUS_LOG_FATAL( "Process node description " + sInp + " cannot be parsed" );
	}

	ProcessNodeType ntype;

	if ( splitted[0] == "filter" ){
		ntype = ProcessNodeType::Filter;
	} else if ( splitted[0] == "producer" ) {
		ntype = ProcessNodeType::Producer;
	} else {
		ARTUS_LOG_FATAL("process node type " + splitted[0] + " is unknown" );
	}

	return std::make_pair( ntype, splitted[1]);
}
