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

/*
	This example implements a simple dummy anaylsis which
	reads entries from a root file and produces various pt plots

	It can be run with the config file data/exampleConfig.json
*/

int main(int argc, char** argv) {

	// parse the command line and load the
	ArtusConfig myConfig(argc, argv);

	// create the output root file
	boost::scoped_ptr < TFile
			> rootOutputFile(
					new TFile(myConfig.GetOutputPath().c_str(), "RECREATE"));

	// will load the Ntuples from the root file
	// this must be modified if you want to load more/new quantities
	TraxEventProvider evtProvider(myConfig.GetInputFiles());

	// the pipeline initializer will setup the pipeline, with
	// all the attached Producer, Filer and Consumer
	TraxPipelineInitializer pInit;

	TraxPipelineRunner runner;

	// add global meta producers
	runner.AddGlobalMetaProducer(new PtCorrectionProducer());

	// load the pipeline with their configuration from the config file
	myConfig.LoadPipelines(pInit, runner, rootOutputFile.get());

	// load the global settings from the config file
	TraxGlobalSettings global_settings = myConfig.GetGlobalSettings<
			TraxGlobalSettings>();

	// run all the configured pipelines and all their attached
	// consumers
	runner.RunPipelines<TraxTypes>(evtProvider, global_settings);

	// close output root file, pointer will be automatically
	// deleted
	rootOutputFile->Close();

	return 0;
}
