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
#include "Artus/Example/interface/TraxPipelineInitializer.h"

#include "Artus/Example/interface/PtCorrectionProducer.h"

int main(int argc, char** argv) {

	// parse the command line and load the
	ArtusConfig myConfig(argc, argv);

	// create the output root file
	boost::scoped_ptr < TFile
			> rootOutputFile(
					new TFile(myConfig.GetOutputPath().c_str(), "RECREATE"));

	TraxEventProvider evtProvider(myConfig.GetInputFiles());

	TraxPipelineInitializer pInit;
	TraxPipelineRunner runner;

	// add global meta producers
	runner.AddGlobalMetaProducer(new PtCorrectionProducer());

	myConfig.LoadPipelines(pInit, runner, rootOutputFile.get());

	TraxGlobalSettings global_settings = myConfig.GetGlobalSettings<
			TraxGlobalSettings>();

	runner.RunPipelines<TraxTypes>(evtProvider, global_settings);

	// close output root file, pointer will be automatically
	// deleted
	rootOutputFile->Close();

	return 0;
}
