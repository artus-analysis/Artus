/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <TFile.h>

#include "Artus/Configuration/interface/ArtusConfig.h"

#include "Artus/Example/interface/TraxTypes.h"
#include "Artus/Example/interface/TraxEventProvider.h"
#include "Artus/Example/interface/TraxPipelineRunner.h"
#include "Artus/Example/interface/TraxPipelineInitializer.h"

int main(int argc, char** argv) {

	// parse the command line and load the
	ArtusConfig myConfig(argc, argv);

	// create the output root file
	boost::scoped_ptr < TFile
			> rootOutputFile(
					new TFile(myConfig.GetOutputPath().c_str(), "RECREATE"));

	TraxEventProvider evtProvider(myConfig.getInputFiles());

	TraxPipelineInitilizer pInit;
	TraxPipelineRunner runner;

	myConfig.loadPipelines(pInit, runner, rootOutputFile.get());

	// fix me
	TraxPipelineSettings settings;

	runner.RunPipelines<TraxTypes>(evtProvider, settings);

	rootOutputFile->Close();

	return 0;
}
