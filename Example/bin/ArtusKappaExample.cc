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
#include "Artus/Configuration/interface/RootEnvironment.h"

#include "Artus/Example/interface/TraxTypes.h"
#include "Artus/Example/interface/TraxKappaEventProvider.h"
#include "Artus/Example/interface/TraxPipelineInitializer.h"

#include "Artus/Example/interface/PtCorrectionProducer.h"

/*
 This example implements a simple dummy anaylsis which
 reads entries from a root file and produces various pt plots

 It can be run with the config file data/exampleConfig.json
 */

int main(int argc, char** argv)
{

	// parse the command line and load the
	ArtusConfig myConfig(argc, argv);
	// load the global settings from the config file
	TraxSettings global_settings = myConfig.GetGlobalSettings<TraxSettings>();

	// create the output root environment, automatically saves the config into the root file
	RootEnvironment rootEnv(myConfig);

	// will load the Ntuples from the root file
	// this must be modified if you want to load more/new quantities
	FileInterface2 fileInterface(myConfig.GetInputFiles());
	TraxKappaEventProvider evtProvider(fileInterface, McInput);
	evtProvider.WireEvent(global_settings);

	// the pipeline initializer will setup the pipeline, with
	// all the attached Producer, Filer and Consumer
	TraxPipelineInitializer pInit;

	TraxPipelineRunner runner;

	// add global producers
	runner.AddGlobalProducer(new PtCorrectionProducer());

	// load the pipeline with their configuration from the config file
	myConfig.LoadPipelines(pInit, runner, rootEnv.GetRootFile());

	// run all the configured pipelines and all their attached
	// consumers
	runner.RunPipelines<TraxTypes>(evtProvider, global_settings);

	// close output root file
	rootEnv.Close();

	return 0;
}
